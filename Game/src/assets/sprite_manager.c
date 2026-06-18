#include "../../include/sprite_manager.h"
#include <stddef.h>

// ============================================================================
// GERENCIADOR DE SPRITES — implementação
// ----------------------------------------------------------------------------
// Mantém um array de Texture2D indexado por SpriteID e a tabela de caminhos
// esperados. O carregamento é tolerante: se o PNG não existir, a textura fica
// com id == 0 (inválida) e o jogo recorre ao desenho procedural (fallback).
// ============================================================================

// Caminhos esperados de cada sprite (relativos à pasta Game/, onde o jogo roda).
// Mantenha em sincronia com o enum SpriteID em sprite_manager.h.
static const char *SPRITE_PATHS[SPRITE_COUNT] = {
    // Player (por skin)
    [SPR_PLAYER_DEFAULT]  = "Assets/Sprites/Player/anticorpo_default.png",
    [SPR_PLAYER_MEDIC]    = "Assets/Sprites/Player/anticorpo_medica.png",
    [SPR_PLAYER_INFECTED] = "Assets/Sprites/Player/anticorpo_infectada.png",

    // Inimigos — Bactéria
    [SPR_BACT_MELEE]  = "Assets/Sprites/Enemies/Bacteria/melee.png",
    [SPR_BACT_RANGED] = "Assets/Sprites/Enemies/Bacteria/ranged.png",
    [SPR_BACT_BOSS]   = "Assets/Sprites/Enemies/Bacteria/boss.png",

    // Inimigos — Vírus
    [SPR_VIRUS_MELEE]  = "Assets/Sprites/Enemies/Virus/melee.png",
    [SPR_VIRUS_RANGED] = "Assets/Sprites/Enemies/Virus/ranged.png",
    [SPR_VIRUS_BOSS]   = "Assets/Sprites/Enemies/Virus/boss.png",
    [SPR_VIRUS_SHIELD] = "Assets/Sprites/Enemies/Virus/shield.png",

    // Armas
    [SPR_WEAPON_SYRINGE_SWORD] = "Assets/Sprites/Weapons/espada_seringa.png",
    [SPR_WEAPON_PHAGE_RIFLE]   = "Assets/Sprites/Weapons/rifle_bacteriofagos.png",
    [SPR_WEAPON_VACCINE_RIFLE] = "Assets/Sprites/Weapons/rifle_vacina.png",
    [SPR_WEAPON_SCALPEL]       = "Assets/Sprites/Weapons/escalpelizador.png",

    // Itens
    [SPR_ITEM_MASK]        = "Assets/Sprites/Items/mascara.png",
    [SPR_ITEM_DISTANCING]  = "Assets/Sprites/Items/distanciamento.png",
    [SPR_ITEM_RNA_GRENADE] = "Assets/Sprites/Items/granada_rna.png",
    [SPR_ITEM_CYTOKINE]    = "Assets/Sprites/Items/citocina.png",

    // Projéteis
    [SPR_PROJ_PLAYER] = "Assets/Sprites/Projectiles/player_proj.png",
    [SPR_PROJ_ENEMY]  = "Assets/Sprites/Projectiles/enemy_proj.png",

    // Mapa
    [SPR_MAP_BODY]       = "Assets/Sprites/Map/corpo.png",
    [SPR_MAP_SILHOUETTE] = "Assets/Sprites/Map/silhueta.png",
    [SPR_MAP_ORGANS]     = "Assets/Sprites/Map/orgaos.png",

    // UI
    [SPR_UI_ICONS] = "Assets/Sprites/UI/icones.png",
};

static Texture2D s_sprites[SPRITE_COUNT] = {0};
static bool s_loaded[SPRITE_COUNT] = {0};

void LoadSprites(void)
{
    for (int i = 0; i < SPRITE_COUNT; i++)
    {
        const char *path = SPRITE_PATHS[i];
        s_loaded[i] = false;
        s_sprites[i] = (Texture2D){0};

        // Só tenta carregar se o arquivo existir (evita avisos e falhas da raylib).
        if (path != NULL && FileExists(path))
        {
            Texture2D tex = LoadTexture(path);
            if (tex.id != 0)
            {
                SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
                s_sprites[i] = tex;
                s_loaded[i] = true;
            }
        }
    }
}

void UnloadSprites(void)
{
    for (int i = 0; i < SPRITE_COUNT; i++)
    {
        if (s_loaded[i] && s_sprites[i].id != 0)
        {
            UnloadTexture(s_sprites[i]);
        }
        s_loaded[i] = false;
        s_sprites[i] = (Texture2D){0};
    }
}

bool SpriteAvailable(SpriteID id)
{
    if (id < 0 || id >= SPRITE_COUNT) return false;
    return s_loaded[id];
}

Texture2D GetSprite(SpriteID id)
{
    if (id < 0 || id >= SPRITE_COUNT) return (Texture2D){0};
    return s_sprites[id];
}

void DrawSpriteCentered(SpriteID id, Vector2 center, Vector2 destSize, float rotation, Color tint)
{
    if (!SpriteAvailable(id)) return;

    Texture2D tex = s_sprites[id];
    Rectangle src = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Rectangle dst = { center.x, center.y, destSize.x, destSize.y };
    Vector2 origin = { destSize.x * 0.5f, destSize.y * 0.5f }; // pivô no centro
    DrawTexturePro(tex, src, dst, origin, rotation, tint);
}

bool DrawSpriteOrFallback(SpriteID id, Vector2 center, Vector2 destSize, float rotation, Color tint,
                          void (*fallbackProc)(void *userData), void *userData)
{
    if (SpriteAvailable(id))
    {
        DrawSpriteCentered(id, center, destSize, rotation, tint);
        return true;
    }

    // Sem PNG: mantém o desenho procedural atual.
    if (fallbackProc != NULL) fallbackProc(userData);
    return false;
}
