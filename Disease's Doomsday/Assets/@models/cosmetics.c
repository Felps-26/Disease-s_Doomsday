// cosmetics.c
// Tabelas de dados do guarda-roupa modular (peças por slot). Tema biológico/
// imunológico: biossegurança, mutações, equipamento médico, plasma/DNA.
// Índice 0 de cada slot é sempre "nenhum/padrão".
#include "cosmetics.h"

#define C_WHITE  (Color){ 235, 240, 250, 255 }
#define C_CYAN   (Color){ 0, 200, 255, 255 }
#define C_GREEN  (Color){ 0, 200, 120, 255 }
#define C_PURPLE (Color){ 150, 90, 210, 255 }
#define C_MAG    (Color){ 230, 80, 200, 255 }
#define C_GRAY   (Color){ 150, 165, 180, 255 }
#define C_GOLD   (Color){ 230, 180, 50, 255 }

static const CosmeticItem HELMET[] = {
    { "Nenhum",                "Cabeca descoberta.",                     1, C_GRAY },
    { "Capacete de Contencao", "Casca branca de biosseguranca.",         1, C_WHITE },
    { "Elmo de Quitina",       "Carapaca mutante endurecida.",           3, C_GREEN },
    { "Visor Microscopico",    "Optica de aumento ciano sobre os olhos.",5, C_CYAN },
};
static const CosmeticItem FACE[] = {
    { "Nenhum",                "Sem acessorio facial.",                  1, C_GRAY },
    { "Mascara N95",           "Protecao respiratoria hospitalar.",      1, C_WHITE },
    { "Respirador de Plasma",  "Filtro energizado contra patogenos.",    4, C_MAG },
    { "Filtro de Anticorpos",  "Membrana imune que purifica o ar.",      6, C_GREEN },
};
static const CosmeticItem CHEST[] = {
    { "Peitoral Padrao",       "Blindagem basica do anticorpo.",         1, C_GRAY },
    { "Placa Celular",         "Membrana reforcada em camadas.",         1, C_CYAN },
    { "Nucleo de DNA",         "Helice luminosa no peito.",              4, C_GREEN },
    { "Coldre Medico",         "Bolsos de campo para suprimentos.",      2, C_GOLD },
};
static const CosmeticItem ARMS[] = {
    { "Bracos Padrao",         "Bracos sem acessorio.",                  1, C_GRAY },
    { "Luvas Cirurgicas",      "Latex esteril, aderencia precisa.",      1, C_WHITE },
    { "Bracadeiras de Quitina","Reforco mutante nos antebracos.",        3, C_GREEN },
};
static const CosmeticItem LEGS[] = {
    { "Calcas Padrao",         "Protecao basica das pernas.",            1, C_GRAY },
    { "Perneiras de Contencao","Vedacao biossegura.",                    1, C_WHITE },
    { "Musculatura Mutante",   "Fibra reforcada, salto potente.",        5, C_PURPLE },
};
static const CosmeticItem BOOTS[] = {
    { "Botas Padrao",          "Calcado basico.",                        1, C_GRAY },
    { "Botas Estereis",        "Solado limpo, sem contaminacao.",        1, C_WHITE },
    { "Cascos Mutantes",       "Garras de adesao a qualquer tecido.",    4, C_GREEN },
};
static const CosmeticItem BACK[] = {
    { "Nenhum",                "Costas livres.",                         1, C_GRAY },
    { "Tanque de Citocina",    "Reserva regenerativa nas costas.",       1, C_GREEN },
    { "Asas de Plasma",        "Propulsao energetica em arco.",          6, C_CYAN },
    { "Mochila de Laboratorio","Kit de campo do anticorpo.",             2, C_GOLD },
};
static const CosmeticItem FXP[] = {
    { "Nenhum",                "Sem efeito visual.",                     1, C_GRAY },
    { "Aura de Anticorpos",    "Halo imune pulsante ao redor.",          1, C_CYAN },
    { "Helice de DNA",         "Fitas de DNA orbitando o corpo.",        4, C_GREEN },
    { "Particulas de Plasma",  "Brasas energeticas em suspensao.",       7, C_MAG },
};

typedef struct SlotDef { const char *name; const char *shortName; const CosmeticItem *items; int count; } SlotDef;

#define ARR(a) (a), (int)(sizeof(a)/sizeof((a)[0]))
static const SlotDef SLOTS[COS_SLOT_COUNT] = {
    [COS_HELMET] = { "Capacete",            "Capacete", ARR(HELMET) },
    [COS_FACE]   = { "Mascara / Facial",    "Facial",   ARR(FACE) },
    [COS_CHEST]  = { "Peitoral",            "Peitoral", ARR(CHEST) },
    [COS_ARMS]   = { "Bracos / Luvas",      "Bracos",   ARR(ARMS) },
    [COS_LEGS]   = { "Calcas",              "Calcas",   ARR(LEGS) },
    [COS_BOOTS]  = { "Botas",               "Botas",    ARR(BOOTS) },
    [COS_BACK]   = { "Acessorio Traseiro",  "Traseiro", ARR(BACK) },
    [COS_FX]     = { "Efeito Visual",       "Efeito",   ARR(FXP) },
};
#undef ARR

const char *CosmeticSlotName(CosmeticSlot s)
{
    if (s < 0 || s >= COS_SLOT_COUNT) return "?";
    return SLOTS[s].name;
}
const char *CosmeticSlotShort(CosmeticSlot s)
{
    if (s < 0 || s >= COS_SLOT_COUNT) return "?";
    return SLOTS[s].shortName;
}
int CosmeticItemCount(CosmeticSlot s)
{
    if (s < 0 || s >= COS_SLOT_COUNT) return 0;
    return SLOTS[s].count;
}
const CosmeticItem *CosmeticGet(CosmeticSlot s, int idx)
{
    if (s < 0 || s >= COS_SLOT_COUNT) return 0;
    if (idx < 0 || idx >= SLOTS[s].count) return 0;
    return &SLOTS[s].items[idx];
}
bool CosmeticUnlocked(CosmeticSlot s, int idx, int playerLevel)
{
    const CosmeticItem *it = CosmeticGet(s, idx);
    if (!it) return false;
    return playerLevel >= it->unlockLevel;
}
