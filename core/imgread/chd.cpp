#include "common.h"

#include "deps/libchdr/include/libchdr/chd.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

/* tracks are padded to a multiple of this many frames */
const uint32_t CD_TRACK_PADDING = 4;

// Leitura antecipada (FC_CHD_PREFETCH=0 desliga): cada hunk lido pede os
// seguintes a uma thread que os descomprime (LZMA/FLAC + ECC) num cache LRU.
// A descompressao na emu thread era ~15% dela no Shenmue II (streaming da
// cidade). Mesmos bytes; o tempo emulado do GD-ROM nao depende do host.
struct CHDDisc : Disc
{
	chd_file* chd;
	u8* hunk_mem;
	u32 old_hunk;

	u32 hunkbytes;
	u32 sph;

	static const int NSLOTS = 32;
	static const int AHEAD = 8;
	struct Slot { u32 hunk; bool ready; u64 lru; u8 *mem; };
	Slot slots[NSLOTS];
	u64 clock = 0;
	std::mutex mx;			// cache e fila
	std::condition_variable cvWork, cvReady;
	std::deque<u32> queue;
	// chd_read nao e reentrante num mesmo handle: cada thread de
	// descompressao abre o seu (arquivo so de leitura); a emu thread usa o
	// principal. Uma thread por padrao (FC_CHD_WORKERS): no Shenmue II com duas
	// a velocidade caiu (54,6% contra 61,2%): disputam nucleos e banda de
	// memoria com a emu thread. Alcance maior (32) tambem nao ajudou.
	static const int NWORKERS = 4;
	int nworkers = 1;
	std::thread workers[NWORKERS];
	chd_file *wchd[NWORKERS] = {};
	bool quit = false;
	bool prefetch = false;
	u32 totalHunks = 0;
	u64 nRead = 0, nHit = 0, nWait = 0, nMiss = 0;

	CHDDisc()
	{
		chd=0;
		hunk_mem=0;
		for (Slot &sl : slots) { sl.hunk = ~0u; sl.ready = false; sl.lru = 0; sl.mem = nullptr; }
	}

	bool TryOpen(const char* file);

	Slot *find(u32 hunk)
	{
		for (Slot &sl : slots)
			if (sl.hunk == hunk)
				return &sl;
		return nullptr;
	}
	// slot pronto (ou vazio) usado ha mais tempo; nunca um em descompressao
	Slot *victim()
	{
		Slot *best = nullptr;
		for (Slot &sl : slots)
			if ((sl.hunk == ~0u || sl.ready) && (best == nullptr || sl.lru < best->lru))
				best = &sl;
		return best;
	}
	void decode(chd_file *h, Slot *sl, u32 hunk, std::unique_lock<std::mutex> &l)
	{
		sl->hunk = hunk;
		sl->ready = false;
		l.unlock();
		chd_read(h, hunk, sl->mem);
		l.lock();
		sl->ready = true;
		cvReady.notify_all();
	}
	void workerLoop(chd_file *hchd)
	{
		std::unique_lock<std::mutex> l(mx);
		for (;;)
		{
			cvWork.wait(l, [this] { return quit || !queue.empty(); });
			if (quit)
				return;
			u32 h = queue.front();
			queue.pop_front();
			if (find(h) != nullptr)
				continue;
			chd_file *hd = hchd;
			Slot *sl = victim();
			if (sl == nullptr)
				continue;
			sl->lru = ++clock - NSLOTS / 2;	// antecipado: sai antes de um usado
			decode(hd, sl, h, l);
		}
	}
	// copia um setor do hunk (emu thread)
	void readSector(u32 hunk, u32 ofs, u8 *dst, u32 len)
	{
		std::unique_lock<std::mutex> l(mx);
		nRead++;
		Slot *sl = find(hunk);
		if (sl != nullptr && sl->ready)
			nHit++;
		else if (sl != nullptr)
		{
			nWait++;		// a thread esta descomprimindo este: espera
			cvReady.wait(l, [sl, hunk] { return sl->hunk != hunk || sl->ready; });
			if (sl->hunk != hunk)
				sl = nullptr;
		}
		if (sl == nullptr || !sl->ready || sl->hunk != hunk)
		{
			nMiss++;
			sl = victim();
			decode(chd, sl, hunk, l);
		}
		sl->lru = ++clock;
		memcpy(dst, sl->mem + ofs, len);
		// pede os proximos
		bool pushed = false;
		for (int k = 1; k <= AHEAD; k++)
		{
			u32 h = hunk + k;
			if (h >= totalHunks || find(h) != nullptr)
				continue;
			if (std::find(queue.begin(), queue.end(), h) != queue.end())
				continue;
			queue.push_back(h);
			pushed = true;
		}
		while (queue.size() > AHEAD * 2)
			queue.pop_front();
		if (pushed)
			cvWork.notify_one();
		if ((nRead & 16383) == 0)
			fprintf(stderr, "chd: %llu setores, %.1f%% ja prontos, %llu esperando a thread, %llu descomprimidos na emu thread\n",
					(unsigned long long)nRead, 100.0 * nHit / nRead, (unsigned long long)nWait, (unsigned long long)nMiss);
	}

	~CHDDisc()
	{
		if (prefetch)
		{
			{
				std::lock_guard<std::mutex> l(mx);
				quit = true;
			}
			cvWork.notify_all();
			for (int i = 0; i < NWORKERS; i++)
			{
				if (workers[i].joinable())
					workers[i].join();
				if (wchd[i] != nullptr)
					chd_close(wchd[i]);
			}
		}
		for (Slot &sl : slots)
			delete [] sl.mem;
		if (hunk_mem)
			delete [] hunk_mem;
		if (chd)
			chd_close(chd);
	}
};

struct CHDTrack : TrackFile
{
	CHDDisc* disc;
	u32 StartFAD;
	u32 Offset;
	u32 fmt;
	bool swap_bytes;

	CHDTrack(CHDDisc* disc, u32 StartFAD,u32 Offset, u32 fmt, bool swap_bytes)
	{
		this->disc=disc;
		this->StartFAD=StartFAD;
		this->Offset=Offset;
		this->fmt=fmt;
		this->swap_bytes = swap_bytes;
	}

	virtual void Read(u32 FAD,u8* dst,SectorFormat* sector_type,u8* subcode,SubcodeFormat* subcode_type)
	{
		u32 fad_offs = FAD + Offset;
		u32 hunk=(fad_offs)/disc->sph;
		u32 hunk_ofs=fad_offs%disc->sph;

		if (disc->prefetch)
			disc->readSector(hunk, hunk_ofs * (2352 + 96), dst, fmt);
		else
		{
			if (disc->old_hunk!=hunk)
			{
				chd_read(disc->chd,hunk,disc->hunk_mem); //CHDERR_NONE
				disc->old_hunk = hunk;
			}
			memcpy(dst,disc->hunk_mem+hunk_ofs*(2352+96),fmt);
		}

		if (swap_bytes)
		{
			for (int i = 0; i < fmt; i += 2)
			{
				u8 b = dst[i];
				dst[i] = dst[i + 1];
				dst[i + 1] = b;
			}
		}

		*sector_type=fmt==2352?SECFMT_2352:SECFMT_2048_MODE1;

		//While space is reserved for it, the images contain no actual subcodes
		//memcpy(subcode,disc->hunk_mem+hunk_ofs*(2352+96)+2352,96);
		*subcode_type=SUBFMT_NONE;
	}
};

bool CHDDisc::TryOpen(const char* file)
{
	chd_error err=chd_open(file,CHD_OPEN_READ,0,&chd);

	if (err!=CHDERR_NONE)
	{
		INFO_LOG(GDROM, "chd: chd_open failed for file %s: %d", file, err);
		return false;
	}

	INFO_LOG(GDROM, "chd: parsing file %s", file);

	const chd_header* head = chd_get_header(chd);

	hunkbytes = head->hunkbytes;
	hunk_mem = new u8[hunkbytes];
	old_hunk=0xFFFFFFF;

	sph = hunkbytes/(2352+96);
	totalHunks = head->totalhunks;
	{
		const char *e = getenv("FC_CHD_PREFETCH");
		prefetch = e == nullptr || atoi(e) != 0;
	}
	if (prefetch)
	{
		for (Slot &sl : slots)
			sl.mem = new u8[hunkbytes];
		if (const char *w = getenv("FC_CHD_WORKERS"))
			nworkers = std::max(1, std::min(NWORKERS, atoi(w)));
		for (int i = 0; i < nworkers; i++)
		{
			if (chd_open(file, CHD_OPEN_READ, 0, &wchd[i]) != CHDERR_NONE)
			{
				wchd[i] = nullptr;
				continue;
			}
			chd_file *h = wchd[i];
			workers[i] = std::thread([this, h] { workerLoop(h); });
		}
	}

	if (hunkbytes%(2352+96)!=0)
	{
		INFO_LOG(GDROM, "chd: hunkbytes is invalid, %d\n",hunkbytes);
		return false;
	}

	u32 tag;
	u8 flags;
	char temp[512];
	u32 temp_len;
	u32 total_frames = 150;

	u32 Offset = 0;

	for(;;)
	{
		char type[16], subtype[16], pgtype[16], pgsub[16];
		int tkid=-1,frames=0,pregap=0,postgap=0, padframes=0;

		err = chd_get_metadata(chd, CDROM_TRACK_METADATA2_TAG, tracks.size(), temp, sizeof(temp), &temp_len, &tag, &flags);
		if (err == CHDERR_NONE)
		{
			//"TRACK:%d TYPE:%s SUBTYPE:%s FRAMES:%d PREGAP:%d PGTYPE:%s PGSUB:%s POSTGAP:%d"
			sscanf(temp, CDROM_TRACK_METADATA2_FORMAT, &tkid, type, subtype, &frames, &pregap, pgtype, pgsub, &postgap);
		}
		else if (CHDERR_NONE == (err = chd_get_metadata(chd, CDROM_TRACK_METADATA_TAG, tracks.size(), temp, sizeof(temp), &temp_len, &tag, &flags)) )
		{
			//CDROM_TRACK_METADATA_FORMAT	"TRACK:%d TYPE:%s SUBTYPE:%s FRAMES:%d"
			sscanf(temp, CDROM_TRACK_METADATA_FORMAT, &tkid, type, subtype, &frames);
		}
		else
		{
			err = chd_get_metadata(chd, GDROM_OLD_METADATA_TAG, tracks.size(), temp, sizeof(temp), &temp_len, &tag, &flags);
			if (err != CHDERR_NONE)
			{
				err = chd_get_metadata(chd, GDROM_TRACK_METADATA_TAG, tracks.size(), temp, sizeof(temp), &temp_len, &tag, &flags);
			}

			if (err == CHDERR_NONE)
			{
				//GDROM_TRACK_METADATA_FORMAT	"TRACK:%d TYPE:%s SUBTYPE:%s FRAMES:%d PAD:%d PREGAP:%d PGTYPE:%s PGSUB:%s POSTGAP:%d"
				sscanf(temp, GDROM_TRACK_METADATA_FORMAT, &tkid, type, subtype, &frames, &padframes, &pregap, pgtype, pgsub, &postgap);
			}
			else
			{
				break;
			}
		}

		if (tkid!=(tracks.size()+1) || (strcmp(type,"MODE1_RAW")!=0 && strcmp(type,"AUDIO")!=0 && strcmp(type,"MODE1")!=0) || strcmp(subtype,"NONE")!=0 || pregap!=0 || postgap!=0)
		{
			INFO_LOG(GDROM, "chd: track type %s is not supported", type);
			return false;
		}
		DEBUG_LOG(GDROM, "%s", temp);
		Track t;
      t.StartFAD = total_frames;
		total_frames += frames;
		t.EndFAD = total_frames - 1;
		t.ADDR = 0;
		t.CTRL = strcmp(type,"AUDIO") == 0 ? 0 : 4;
		t.file = new CHDTrack(this, t.StartFAD, Offset - t.StartFAD, strcmp(type,"MODE1") ? 2352 : 2048, t.CTRL == 0 && head->version >= 5);

		int padded = (frames + CD_TRACK_PADDING - 1) / CD_TRACK_PADDING;
		Offset += padded * CD_TRACK_PADDING;

		tracks.push_back(t);
	}

	if (total_frames!=549300 || tracks.size()<3)
		WARN_LOG(GDROM, "WARNING: chd: Total frames is wrong: %u frames in %zu tracks", total_frames, tracks.size());

	FillGDSession();

	return true;
}


Disc* chd_parse(const char* file)
{
	// Only try to open .chd files
	size_t len = strlen(file);
	if (len > 4 && stricmp( &file[len - 4], ".chd"))
		return nullptr;

	CHDDisc* rv = new CHDDisc();

	if (rv->TryOpen(file))
		return rv;
	else
	{
		delete rv;
		return 0;
	}
}
