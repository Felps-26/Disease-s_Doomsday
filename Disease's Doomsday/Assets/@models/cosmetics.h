// cosmetics.h
// Catálogo DATA-DRIVEN do guarda-roupa modular do Anticorpo.
// Disease's Doomsday — Projeto de Saúde Pública / DF
//
// O slot/quantidade de itens e o estado equipado vivem no Player
// (player.h: cosmetics[COS_SLOT_COUNT]); este módulo descreve cada peça (nome,
// descrição, desbloqueio, cor) e regras, sem condicionais espalhadas pelo jogo.
#ifndef COSMETICS_H
#define COSMETICS_H

#include "../../include/game.h"   // CosmeticSlot, COS_SLOT_COUNT, Color

typedef struct CosmeticItem {
    const char *name;        // nome exibido
    const char *desc;        // descrição curta (tema biológico)
    int   unlockLevel;       // nível mínimo p/ liberar (1 = sempre liberado)
    Color tint;              // cor representativa (swatch da UI)
} CosmeticItem;

// Nome longo/curto do slot (para abas e cabeçalhos da UI).
const char *CosmeticSlotName(CosmeticSlot s);
const char *CosmeticSlotShort(CosmeticSlot s);

// Quantidade de itens do slot (inclui o índice 0 = nenhum/padrão).
int CosmeticItemCount(CosmeticSlot s);

// Item (s, idx). Retorna NULL se inválido.
const CosmeticItem *CosmeticGet(CosmeticSlot s, int idx);

// Item liberado para o nível atual do jogador?
bool CosmeticUnlocked(CosmeticSlot s, int idx, int playerLevel);

#endif // COSMETICS_H
