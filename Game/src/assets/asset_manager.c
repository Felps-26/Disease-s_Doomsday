#include "../../include/asset_manager.h"
#include "../../include/sprite_manager.h"
#include <stdio.h>

GameAssets g_assets = {0};

void LoadGameAssets(void)
{
    InitAudioDevice();

    // Carrega Fonte com suporte a acentuação (PT-BR)
    // ASCII básico (32..126) + Latin-1 com acentos (À..ÿ, 192..255)
    int codepoints[95 + 64];
    int cpCount = 0;
    for (int c = 32; c <= 126; c++) codepoints[cpCount++] = c;
    for (int c = 192; c <= 255; c++) codepoints[cpCount++] = c;

    g_assets.font = LoadFontEx("Assets/font.ttf", 96, codepoints, cpCount);
    if (g_assets.font.texture.id == 0)
    {
        g_assets.font = GetFontDefault();
    }
    else
    {
        SetTextureFilter(g_assets.font.texture, TEXTURE_FILTER_BILINEAR);
    }

    // Carrega Música
    g_assets.musicMain = LoadMusicStream("Assets/Musica/DeepVoid.mp3");
    g_assets.musicB = LoadMusicStream("Assets/Musica/DeepVoid.mp3");
    g_assets.sfxAttack = LoadSound("Assets/Musica/Ataque_Pulse.mp3");
    g_assets.sfxHurt = LoadSound("Assets/Musica/Dano_Heroi.mp3");
    // BUGFIX: Assets/SFX/*.wav nunca existiram, deixando coleta de itens e morte
    // sem nenhum retorno sonoro. Reaproveitamos SFX existentes para restaurar o
    // feedback (idealmente substituir por sons dedicados de pickup/morte).
    g_assets.sfxPickup = LoadSound("Assets/Musica/Ataque_Pulse.mp3");
    g_assets.sfxDeath = LoadSound("Assets/Musica/Dano_Heroi.mp3");
    
    // Novos efeitos de inimigos
    g_assets.sfxEnemyHurt = LoadSound("Assets/Musica/Dano_enemy.mp3");
    g_assets.sfxEnemyShoot = LoadSound("Assets/Musica/Projétil_enemy.mp3");
    
    // Shader
    g_assets.biologicalShader = LoadShader(0, "Assets/Shaders/biological.fs");
    g_assets.shdLowHP = LoadShader(0, "Assets/Shaders/low_hp_vignette.fs");
    if (g_assets.biologicalShader.id != 0 && g_assets.shdLowHP.id != 0) {
        g_assets.shaderLoaded = true;
        g_assets.shdLowHPTimeLoc = GetShaderLocation(g_assets.shdLowHP, "time");
        g_assets.shdLowHPResLoc = GetShaderLocation(g_assets.shdLowHP, "resolution");
    }

    // Pipeline de sprites: carrega os PNGs que existirem (fallback procedural
    // automático para os que faltarem). A janela/contexto GL já existe aqui.
    LoadSprites();
}

void UnloadGameAssets(void)
{
    if (g_assets.font.texture.id != GetFontDefault().texture.id && g_assets.font.texture.id != 0)
    {
        UnloadFont(g_assets.font);
    }

    if (g_assets.musicMain.stream.buffer != NULL) UnloadMusicStream(g_assets.musicMain);
    if (g_assets.musicB.stream.buffer != NULL) UnloadMusicStream(g_assets.musicB);
    if (g_assets.sfxAttack.stream.buffer != NULL) UnloadSound(g_assets.sfxAttack);
    if (g_assets.sfxHurt.stream.buffer != NULL) UnloadSound(g_assets.sfxHurt);
    if (g_assets.sfxPickup.stream.buffer != NULL) UnloadSound(g_assets.sfxPickup);
    if (g_assets.sfxDeath.stream.buffer != NULL) UnloadSound(g_assets.sfxDeath);
    if (g_assets.sfxEnemyHurt.stream.buffer != NULL) UnloadSound(g_assets.sfxEnemyHurt);
    if (g_assets.sfxEnemyShoot.stream.buffer != NULL) UnloadSound(g_assets.sfxEnemyShoot);

    if (g_assets.shaderLoaded) {
        UnloadShader(g_assets.biologicalShader);
        UnloadShader(g_assets.shdLowHP);
    }

    // Descarrega as texturas da pipeline de sprites (antes de fechar a janela).
    UnloadSprites();

    CloseAudioDevice();
}
