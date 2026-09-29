/*
 * gdcartridge.h
 *
 *  Created on: Nov 16, 2018
 *      Author: flyinghead
 *
 * From mame naomigd.h
 * // license:BSD-3-Clause
 * // copyright-holders:Olivier Galibert
 *
 */

#ifndef CORE_HW_NAOMI_GDCARTRIDGE_H_
#define CORE_HW_NAOMI_GDCARTRIDGE_H_

#include "naomi_cart.h"
#include "imgread/common.h"

#include <vector>

class GDCartridge: public NaomiCartridge {
public:
	GDCartridge(u32 size) : NaomiCartridge(size)
	{
	}
	~GDCartridge()
	{
		free(dimm_data);
		if (gdrom != nullptr)
			delete gdrom;
	}
	virtual void Init() override
	{
		device_start();
		device_reset();
	}
	virtual void* GetDmaPtr(u32 &size) override;
	virtual void AdvancePtr(u32 size) override;
	virtual bool Read(u32 offset, u32 size, void* dst) override;
	virtual std::string GetGameId() override;

	void SetGDRomName(const char *name) { this->gdrom_name = name; }

private:
	enum { FILENAME_LENGTH=24 };

	const char *gdrom_name = nullptr;

	u32 dimm_cur_address = 0;

	u8 *dimm_data = nullptr;
	u32 dimm_data_size = 0;

	// ROM.BIN lido/descriptografado sob demanda (por segmentos de 16KB) em vez
	// de inteiro no boot: o device_start lia ~134MB (cvs2) e descriptografava em
	// DES antes do jogo iniciar, ~20s de tela preta. Portado do upstream.
	std::vector<bool> loadedSegments;
	static constexpr u32 SEGMENT_SIZE = 16 * 1024;
	Disc *gdrom = nullptr;
	u32 file_start = 0;
	u32 des_subkeys[32];

	static const u32 DES_LEFTSWAP[];
	static const u32 DES_RIGHTSWAP[];
	static const u32 DES_SBOX1[];
	static const u32 DES_SBOX2[];
	static const u32 DES_SBOX3[];
	static const u32 DES_SBOX4[];
	static const u32 DES_SBOX5[];
	static const u32 DES_SBOX6[];
	static const u32 DES_SBOX7[];
	static const u32 DES_SBOX8[];
	static const u32 DES_MASK_TABLE[];
	static const u8 DES_ROTATE_TABLE[16];

	void device_start();
	void device_reset();
	void find_file(const char *name, const u8 *dir_sector, u32 &file_start, u32 &file_size);
	void loadSegments(u32 offset, u32 size);

	inline void permutate(u32 &a, u32 &b, u32 m, int shift);
   void des_generate_subkeys(u64 key, u32 *subkeys);
	template<bool decrypt>
	u64 des_encrypt_decrypt(u64 src, const u32 *des_subkeys);
	u64 rev64(u64 src);
	void read_gdrom(Disc *gdrom, u32 sector, u8* dst, u32 count = 1);
};

#endif /* CORE_HW_NAOMI_GDCARTRIDGE_H_ */
