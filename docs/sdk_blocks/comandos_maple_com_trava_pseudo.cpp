// Comandos Maple com trava (`tas.b`) — pseudo-C++ para todos os jogos identificados
// (docs/sdk_blocks/comandos_maple_com_trava.md). API: ver
// luz_e_transformacao_de_vertices_pseudo.cpp.
//
// Não é quente (`tas.b` 0,0% da emu): nativizar não dá velocidade. Serve para entender
// a camada que conversa com o controle/VMU (suspeita do 4.117, "memory card sem espaço").
//
// Variantes:
//   - DOA2 e mais 10 jogos: referência (162 de 162 opcodes).
//   - EGG, Evolution 1, Macross, Napple (126), SA2 (85), Soulcalibur (130): iguais no
//     trecho executado (só têm menos funções compiladas na sessão).
//   - Shenmue (0C1DFC90): **mesmo comportamento, outra ordem de instruções** (outro
//     compilador/versão): os shll16/shll8 e o `mov #2,r7` trocam de lugar e o `add r15`
//     do retorno vem depois. Resultado idêntico; não precisa de if/else, só de outra
//     assinatura.
//
//   Jogo (endereço da 1ª função)   despachante (bsr)
//   DOA2            8C135F50       8C1353B0
//   Shenmue         0C1DFCB0       0C1DF0EC
//   (demais: ver a tabela do .md; o despachante é o alvo do `bsr` de cada função)
//
// Códigos de comando Maple (r5 do despachante): 0x01 informação do dispositivo,
// 0x0A informação da mídia, 0x0B ler bloco, 0x0C gravar bloco, 0x0E ajustar condição.

namespace maple {

enum Cmd : u32 { DEVINFO = 0x01, GETMINFO = 0x0A, BREAD = 0x0B, BWRITE = 0x0C, SETCOND = 0x0E };

struct Variant { u32 base; u32 dispatcher; u32 lockLit; bool shenmueOrder; };

// O despachante do jogo (não reescrito aqui): monta o quadro Maple da porta `port`
// (estrutura de 44 bytes por porta, tabela apontada por um literal) e enfileira.
// Chamar o original preserva todo o comportamento de E/S.
static s32 dispatch(const Variant &v, u32 port, Cmd cmd, const u32 *payload, u32 words,
                    u32 extra0, u32 extra1)
{
	return call_guest(v.dispatcher, port, cmd, guest_ptr(payload), words, extra0, extra1);
}

// tas.b: lê o byte, T = (byte == 0), grava byte | 0x80 — atômico no SH4.
static inline bool try_lock(u32 lockAddr)
{
	const u8 old = read8(lockAddr);
	write8(lockAddr, old | 0x80);
	return old == 0;
}
static inline void unlock(u32 lockAddr) { write8(lockAddr, 0); }

// Uma função por comando; todas têm a mesma forma:
//   trava ocupada → retorna sem fazer nada (o caminho não foi executado nas sessões;
//   provavelmente devolve um código de "ocupado"); livre → monta o pedido e chama.
s32 get_media_info(const Variant &v, u32 port, u32 function, u32 partition)
{
	const u32 lock = ram32(ram32(v.base + v.lockLit));
	if (!try_lock(lock))
		return BUSY;
	const u32 payload[2] = { function, partition << 24 };
	const s32 r = dispatch(v, port, GETMINFO, payload, 2, 0, 0);
	unlock(lock);
	return r;
}

s32 block_read(const Variant &v, u32 port, u32 function, u32 partition, u32 phase, u32 block)
{
	const u32 lock = ram32(ram32(v.base + v.lockLit));
	if (!try_lock(lock))
		return BUSY;
	const u32 payload[2] = { function, (partition << 24) | (phase << 16) | block };
	const s32 r = dispatch(v, port, BREAD, payload, 2, 0, 0);
	unlock(lock);
	return r;
}

s32 block_write(const Variant &v, u32 port, u32 function, u32 partition, u32 phase,
                u32 block, u32 buffer, u32 size)
{
	const u32 lock = ram32(ram32(v.base + v.lockLit));
	if (!try_lock(lock))
		return BUSY;
	const u32 payload[2] = { function, (partition << 24) | (phase << 16) | block };
	const s32 r = dispatch(v, port, BWRITE, payload, 2, buffer, size);
	unlock(lock);
	return r;
}

s32 set_condition(const Variant &v, u32 port, u32 function, u32 a, u32 b)
{
	const u32 lock = ram32(ram32(v.base + v.lockLit));
	if (!try_lock(lock))
		return BUSY;
	const u32 payload[3] = { function, a, b };
	const s32 r = dispatch(v, port, SETCOND, payload, 1, payload[1], payload[2]);
	unlock(lock);
	return r;
}

// Sem trava (a 1ª função da família): pede a informação do dispositivo.
s32 device_info(const Variant &v, u32 port)
{
	return dispatch(v, port, DEVINFO, nullptr, 0, 0, 0);
}

// Reconhecimento: a ordem do Shenmue é outra assinatura para as mesmas funções.
//   if (bytes_match(base, SIG_REFERENCIA)) v.shenmueOrder = false;
//   else if (bytes_match(base, SIG_SHENMUE)) v.shenmueOrder = true;
// O comportamento não muda; o campo só serve para achar os offsets (lockLit etc.).

} // namespace maple
