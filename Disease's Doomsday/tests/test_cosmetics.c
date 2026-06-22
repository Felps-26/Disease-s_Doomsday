// test_cosmetics.c
// Testa o catálogo DATA-DRIVEN do guarda-roupa modular (cosmetics.c): contagem
// de slots/itens, limites, e regras de desbloqueio por nível. Não precisa de
// janela/raylib em runtime (apenas tipos do cabeçalho).
#include "../Assets/@models/cosmetics.h"
#include <stdio.h>
#include <string.h>

static int failures = 0;
#define CHECK(cond, msg) do { if (!(cond)) { printf("  FALHA: %s\n", msg); failures++; } } while (0)

int main(void)
{
    printf("=== TESTE DO GUARDA-ROUPA (cosmetics) ===\n");

    CHECK(COS_SLOT_COUNT == 8, "esperado 8 slots de cosmetico");

    int totalItems = 0;
    for (int s = 0; s < COS_SLOT_COUNT; s++)
    {
        int n = CosmeticItemCount((CosmeticSlot)s);
        CHECK(n >= 1, "cada slot deve ter ao menos 1 item (nenhum/padrao)");
        totalItems += n;

        // Nome do slot válido (não nulo / não vazio).
        const char *sn = CosmeticSlotName((CosmeticSlot)s);
        const char *ss = CosmeticSlotShort((CosmeticSlot)s);
        CHECK(sn && sn[0] != '\0', "nome longo do slot valido");
        CHECK(ss && ss[0] != '\0', "nome curto do slot valido");

        // Item 0 sempre existe; é o "nenhum/padrao" e sempre liberado.
        const CosmeticItem *it0 = CosmeticGet((CosmeticSlot)s, 0);
        CHECK(it0 != NULL, "item 0 deve existir");
        CHECK(it0 && it0->unlockLevel <= 1, "item 0 deve ser liberado desde o nivel 1");
        CHECK(CosmeticUnlocked((CosmeticSlot)s, 0, 1), "item 0 liberado no nivel 1");

        // Cada item tem nome/descrição e unlockLevel >= 1.
        for (int i = 0; i < n; i++)
        {
            const CosmeticItem *it = CosmeticGet((CosmeticSlot)s, i);
            CHECK(it != NULL, "item valido dentro do range");
            if (!it) continue;
            CHECK(it->name && it->name[0] != '\0', "item tem nome");
            CHECK(it->desc && it->desc[0] != '\0', "item tem descricao");
            CHECK(it->unlockLevel >= 1, "unlockLevel >= 1");
            // Regra de desbloqueio: liberado exatamente quando level >= unlockLevel.
            CHECK(CosmeticUnlocked((CosmeticSlot)s, i, it->unlockLevel), "liberado no proprio nivel de desbloqueio");
            if (it->unlockLevel > 1)
                CHECK(!CosmeticUnlocked((CosmeticSlot)s, i, it->unlockLevel - 1), "bloqueado um nivel abaixo");
        }

        // Índices fora do range retornam NULL (robustez).
        CHECK(CosmeticGet((CosmeticSlot)s, -1) == NULL, "indice negativo -> NULL");
        CHECK(CosmeticGet((CosmeticSlot)s, n) == NULL, "indice >= n -> NULL");
    }

    // Slot inválido é tratado com segurança.
    CHECK(CosmeticItemCount((CosmeticSlot)COS_SLOT_COUNT) == 0, "slot invalido -> 0 itens");
    CHECK(CosmeticGet((CosmeticSlot)-1, 0) == NULL, "slot invalido -> NULL");

    // Deve existir ao menos UM item bloqueado em algum nível alto (demonstra
    // progressão/desbloqueio na UI). Procuramos qualquer unlockLevel > 1.
    int gated = 0;
    for (int s = 0; s < COS_SLOT_COUNT; s++)
        for (int i = 0; i < CosmeticItemCount((CosmeticSlot)s); i++)
        {
            const CosmeticItem *it = CosmeticGet((CosmeticSlot)s, i);
            if (it && it->unlockLevel > 1) gated++;
        }
    CHECK(gated > 0, "deve haver itens com desbloqueio por nivel (estado bloqueado)");

    printf("Slots: %d | itens totais: %d | itens com desbloqueio: %d\n", COS_SLOT_COUNT, totalItems, gated);
    if (failures == 0) { printf("RESULTADO: PASSOU (catalogo de cosmeticos consistente)\n"); return 0; }
    printf("RESULTADO: FALHOU (%d verificacoes)\n", failures);
    return 1;
}
