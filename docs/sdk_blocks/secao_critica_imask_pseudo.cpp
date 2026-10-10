// Seção crítica por máscara de interrupção (IMASK = 15) — pseudo-C++ para todos os jogos
// identificados (docs/sdk_blocks/secao_critica_imask.md). API: ver
// luz_e_transformacao_de_vertices_pseudo.cpp.
//
// O trecho em 8C0083F8 está na área de sistema (bootstrap do disco, mesmo endereço em
// todos os jogos): é a inicialização que chama funções do sistema com as interrupções
// mascaradas e decide o caminho pelo resultado. Não é quente (roda no boot).
// O idioma "guarda IMASK / IMASK = 15 / chama / restaura" também aparece dentro do código
// de vários jogos (as outras ocorrências da tabela do .md).
//
// Variantes:
//   - DOA2 (referência) e mais 19 jogos: idênticos (93 de 93 opcodes).
//   - **PSO v2**: depois da 1ª chamada protegida há **duas chamadas a mais** (17
//     instruções: `jsr` com r4/r5 de literais) antes de seguir — inicialização extra do
//     PSO (provavelmente rede/modem). O resto é igual.
//   - cvs2 (Naomi): não é a mesma rotina; só o idioma aparece no código do jogo.

namespace imask {

struct Variant { bool psoExtraInit; };

// O "mutex" do Dreamcast: com um núcleo, mascarar as interrupções basta para que nada
// mais rode até restaurar.
template <typename F>
static inline u32 with_interrupts_masked(F fn)
{
	const u32 old = (sr_full() >> 4) & 0xF;          // stc SR; shlr2 ×2; and #15
	set_sr((sr_full() & SR_IMASK_CLEAR) | 0xF0);      // IMASK = 15
	const u32 r = fn();
	set_sr((sr_full() & SR_IMASK_CLEAR) | (old << 4)); // restaura só o IMASK
	return r;
}

// Rotina de 8C0083F8 (os alvos dos jsr vêm do literal pool da própria rotina)
u32 system_init(const Variant &v)
{
	call_guest(LIT(0x8C008454));                      // preparação, sem máscara
	const u32 st = with_interrupts_masked([] { return call_guest(LIT(0x8C008458)); });
	u32 next;
	if (st == 4)
		next = 9;                                     // caminho do código 4
	else
	{
		if (v.psoExtraInit)                           // PSO v2: duas chamadas a mais
		{
			call_guest(LIT_PSO(0), LIT_PSO(1), (u16)LIT_PSO(2));
			call_guest(LIT_PSO(0), LIT_PSO(3), ram32(LIT_PSO(4)));
		}
		next = other_path(st);                        // trecho não executado nas sessões
	}
	with_interrupts_masked([&] { return call_guest(LIT(0x8C0084D8), next); });
	call_guest(LIT(0x8C0084DC), next);
	call_guest(LIT(0x8C0084E0), 1);
	const u32 a = call_guest(LIT(0x8C0084E4));
	const u32 b = call_guest(LIT(0x8C0084E4));
	const u32 c = call_guest(LIT(0x8C0084E8), a, b);
	return call_guest(LIT(0x8C0084EC), c);
}

// Reconhecimento: a assinatura da referência casa nos dois; o PSO tem as 17 instruções
// a mais logo depois do `bf` em +0x3C:
//   v.psoExtraInit = bytes_match(0x8C00843E, SIG_PSO_EXTRA);

} // namespace imask
