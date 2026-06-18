#include "../../include/telas.h"
#include <stdio.h>
#include <string.h>

// Banco de perguntas do Quiz da Vigilância Epidemiológica / SUS (DF e Brasil).
// Cada pergunta tem 4 alternativas plausíveis, a correta e uma explicação curta.
typedef struct QuizQuestion {
    const char *question;
    const char *options[4];
    int correctOption;
    const char *explanation;
} QuizQuestion;

static QuizQuestion quizDB[] = {
    // ---- Princípios e diretrizes do SUS ----
    { "Qual destes e um PRINCIPIO doutrinario do SUS?",
      {"Lucratividade", "Universalidade", "Exclusividade", "Terceirizacao"}, 1,
      "Universalidade: saude e direito de todos e dever do Estado." },
    { "O principio da INTEGRALIDADE no SUS significa:",
      {"Atender so urgencias", "Cuidar do individuo como um todo (promocao, prevencao e cura)", "Cobrar por consulta", "Atender so quem trabalha"}, 1,
      "Integralidade = acoes de promocao, prevencao e assistencia de forma articulada." },
    { "O principio da EQUIDADE no SUS busca:",
      {"Tratar todos exatamente igual", "Dar mais a quem precisa mais, reduzindo desigualdades", "Atender so quem paga", "Priorizar capitais"}, 1,
      "Equidade trata os desiguais de forma desigual para alcancar justica em saude." },
    { "Sao DIRETRIZES organizativas do SUS:",
      {"Centralizacao e sigilo", "Descentralizacao, regionalizacao e participacao social", "Privatizacao total", "Propaganda e marketing"}, 1,
      "A Lei 8.080/90 e a 8.142/90 estabelecem essas diretrizes." },
    { "A DESCENTRALIZACAO do SUS significa principalmente:",
      {"Tudo decidido em Brasilia", "Direcao unica em cada esfera, com municipios assumindo servicos", "Fim dos municipios", "So o estado decide"}, 1,
      "Cada esfera (Uniao, estado, municipio) tem direcao unica e responsabilidades proprias." },
    { "A HIERARQUIZACAO/REGIONALIZACAO organiza a rede em:",
      {"Niveis de complexidade articulados por regiao", "Filas unicas nacionais", "Apenas hospitais privados", "Postos sem referencia"}, 0,
      "Atencao primaria, media e alta complexidade conectadas em redes regionais." },
    { "O CONTROLE SOCIAL no SUS e exercido principalmente por meio de:",
      {"Conselhos e Conferencias de Saude", "Ouvidoria do Congresso", "Sindicatos patronais", "Empresas de plano de saude"}, 0,
      "Conselhos de Saude sao paritarios e deliberativos (Lei 8.142/90)." },
    { "Nos Conselhos de Saude, os USUARIOS ocupam quantos por cento das vagas?",
      {"10%", "25%", "50% (paridade)", "5%"}, 2,
      "A representacao dos usuarios e paritaria: 50% do total de conselheiros." },

    // ---- Vigilancia, notificacao, epidemiologia ----
    { "A VIGILANCIA EPIDEMIOLOGICA tem como funcao:",
      {"Vender remedios", "Conhecer, detectar e prevenir mudancas nos fatores de doencas", "Fiscalizar transito", "Construir hospitais"}, 1,
      "Ela monitora doencas e agravos para orientar acoes de controle." },
    { "NOTIFICACAO COMPULSORIA e:",
      {"Opcional para o medico", "Comunicacao obrigatoria de doencas/agravos as autoridades", "Uma multa de transito", "Aviso de ferias"}, 1,
      "Certas doencas DEVEM ser notificadas para vigilancia e resposta rapida." },
    { "Qual sistema registra as notificacoes de doencas no Brasil?",
      {"SINAN", "SPED", "SISU", "SIAPE"}, 0,
      "SINAN = Sistema de Informacao de Agravos de Notificacao." },
    { "Qual a diferenca entre ENDEMIA e EPIDEMIA?",
      {"Sao sinonimos", "Endemia e a presenca habitual; epidemia e o aumento acima do esperado", "Endemia e mundial", "Epidemia dura mais que endemia sempre"}, 1,
      "Endemia = base esperada numa regiao; epidemia = elevacao incomum de casos." },
    { "Uma PANDEMIA caracteriza-se por:",
      {"Casos em um bairro", "Epidemia que se espalha por varios paises/continentes", "Doenca rara", "Doenca so de animais"}, 1,
      "Pandemia e a disseminacao mundial de uma doenca, como a COVID-19." },
    { "Um SURTO e:",
      {"Aumento subito de casos num local/grupo especifico", "Doenca cronica", "Vacina nova", "Plano de saude"}, 0,
      "Surto = elevacao localizada de casos (ex.: numa escola ou bairro)." },
    { "VIGILANCIA SANITARIA atua principalmente em:",
      {"Controle de transito", "Risco de produtos, alimentos, servicos e ambientes a saude", "Pagamento de medicos", "Eleicoes"}, 1,
      "Fiscaliza alimentos, medicamentos, servicos de saude e saneantes." },
    { "VIGILANCIA AMBIENTAL em saude monitora:",
      {"Notas fiscais", "Fatores do ambiente que afetam a saude (agua, ar, vetores)", "Transito aereo", "Bolsa de valores"}, 1,
      "Inclui qualidade da agua, contaminantes e controle de vetores e zoonoses." },

    // ---- Atencao primaria / ESF / RAS ----
    { "A ATENCAO PRIMARIA a Saude (APS) e a porta de entrada e atua na:",
      {"So cirurgias", "Promocao, prevencao e cuidado continuo proximo da comunidade", "Alta complexidade apenas", "Pesquisa espacial"}, 1,
      "A APS coordena o cuidado e resolve a maioria dos problemas de saude." },
    { "A Estrategia Saude da Familia (ESF) tem como base a equipe com:",
      {"So um porteiro", "Medico, enfermeiro, tecnicos e Agentes Comunitarios de Saude", "So dentistas", "So administradores"}, 1,
      "A ESF organiza a APS com equipe multiprofissional e territorio definido." },
    { "O Agente Comunitario de Saude (ACS) atua principalmente:",
      {"No centro cirurgico", "Visitando familias do territorio e fazendo a ponte com a UBS", "No laboratorio", "Na farmacia de alto custo"}, 1,
      "O ACS conhece o territorio e aproxima a comunidade dos servicos." },
    { "A REDE DE ATENCAO A SAUDE (RAS) busca:",
      {"Servicos isolados", "Integrar servicos de diferentes niveis de forma coordenada", "So hospitais privados", "Acabar com a APS"}, 1,
      "RAS conecta APS, especializada e hospitalar com continuidade do cuidado." },
    { "O que e a referencia e contrarreferencia?",
      {"Trocar de plano", "Encaminhar a niveis de maior complexidade e retornar para acompanhamento", "Cancelar consulta", "Pagar exame"}, 1,
      "Garante a continuidade do cuidado entre os niveis de atencao." },

    // ---- Vacinacao ----
    { "A vacinacao em massa protege a comunidade pelo efeito de:",
      {"Imunidade de rebanho (coletiva)", "Efeito estufa", "Efeito placebo", "Efeito domino"}, 0,
      "Quando muitos estao imunes, a circulacao do agente diminui e protege todos." },
    { "O Programa Nacional de Imunizacoes (PNI) e responsavel por:",
      {"Vender vacinas caras", "Oferecer vacinas gratuitas a populacao pelo SUS", "Importar carros", "Controlar o cambio"}, 1,
      "O PNI e referencia mundial em vacinacao publica e gratuita." },
    { "A vacina da Gripe (Influenza) deve ser tomada:",
      {"Uma vez na vida", "Anualmente, pois o virus sofre mutacoes", "A cada 10 anos", "Nunca"}, 1,
      "A composicao muda a cada ano conforme as cepas circulantes." },
    { "Caderneta de vacinacao em dia serve para:",
      {"Decoracao", "Comprovar e acompanhar as doses recebidas ao longo da vida", "Comprar remedio", "Votar"}, 1,
      "Ajuda equipe e usuario a controlar o calendario vacinal." },

    // ---- Dengue / Aedes / arboviroses ----
    { "Qual o principal vetor da Dengue, Zika e Chikungunya?",
      {"Barbeiro", "Aedes aegypti", "Mosquito-palha", "Carrapato"}, 1,
      "O Aedes aegypti transmite essas arboviroses urbanas." },
    { "A melhor forma de evitar criadouros do Aedes e:",
      {"Manter agua parada coberta ou eliminada", "Deixar pratos de planta cheios", "Acumular pneus", "Guardar garrafas abertas"}, 0,
      "Sem agua parada, o mosquito nao se reproduz." },
    { "Sinais de alarme da Dengue grave incluem:",
      {"Espirros", "Dor abdominal intensa e sangramentos", "Unhas fracas", "Soluco"}, 1,
      "Dor abdominal, vomitos persistentes e sangramento exigem atendimento imediato." },
    { "Onde descartar pneus velhos para evitar a Dengue?",
      {"Terreno baldio", "Ecopontos e locais cobertos", "Rios", "Quintal a ceu aberto"}, 1,
      "Pneus acumulam agua e viram criadouro; descarte correto evita focos." },
    { "A Doenca de Chagas e transmitida classicamente pelo:",
      {"Aedes", "Barbeiro (triatomineo)", "Caramujo", "Carrapato"}, 1,
      "O 'barbeiro' transmite o Trypanosoma cruzi." },

    // ---- Atribuicoes das esferas / financiamento ----
    { "Cabe principalmente ao MUNICIPIO no SUS:",
      {"Politica externa", "Executar e gerir as acoes e servicos locais de saude", "Emitir moeda", "Controlar fronteiras"}, 1,
      "O municipio e o principal executor das acoes de saude (APS)." },
    { "Cabe a UNIAO no SUS, sobretudo:",
      {"Coordenacao nacional, normas e cofinanciamento", "Limpar ruas", "Consulta individual", "Vender planos"}, 0,
      "A Uniao formula politicas nacionais e participa do financiamento." },
    { "O financiamento do SUS e responsabilidade:",
      {"So do governo federal", "Das tres esferas: Uniao, estados e municipios", "So dos municipios", "Da iniciativa privada"}, 1,
      "E um financiamento tripartite, definido em lei." },
    { "A RENAME e:",
      {"Lista de hospitais", "Relacao Nacional de Medicamentos Essenciais do SUS", "Imposto", "Cadastro de medicos"}, 1,
      "Orienta a oferta de medicamentos essenciais na rede publica." },
    { "A RENASES e:",
      {"Relacao Nacional de Acoes e Servicos de Saude do SUS", "Um remedio", "Um conselho", "Um exame"}, 0,
      "Define o rol de acoes e servicos oferecidos pelo SUS." },
    { "O Cartao Nacional de Saude (CNS) serve para:",
      {"Identificar o usuario e integrar seus atendimentos no SUS", "Comprar passagens", "Pagar conta de luz", "Tirar passaporte"}, 0,
      "Vincula o cidadao a seus registros e facilita a continuidade do cuidado." },

    // ---- Promocao/prevencao e determinantes ----
    { "DETERMINANTES SOCIAIS da saude sao:",
      {"So genetica", "Condicoes como moradia, renda, educacao e saneamento que afetam a saude", "So virus", "So idade"}, 1,
      "A saude depende fortemente das condicoes de vida e trabalho." },
    { "PROMOCAO da saude difere de PREVENCAO porque:",
      {"Sao iguais", "Promocao melhora condicoes/qualidade de vida; prevencao evita doencas especificas", "Promocao e cirurgia", "Prevencao e propaganda"}, 1,
      "Promocao atua sobre determinantes; prevencao reduz risco de agravos especificos." },
    { "Saneamento basico impacta a saude porque:",
      {"Nao impacta", "Agua tratada e esgoto reduzem doencas infecciosas e parasitarias", "So embeleza a cidade", "So afeta o transito"}, 1,
      "Saneamento e uma das maiores medidas de saude publica da historia." },
    { "Por que NAO usar antibiotico sem prescricao?",
      {"E caro", "Favorece bacterias resistentes (como a KPC) e atrapalha o tratamento", "Tem gosto ruim", "Mancha os dentes"}, 1,
      "O uso indevido seleciona superbacterias resistentes — um risco global." },
    { "Medida eficaz para frear epidemia respiratoria:",
      {"Compartilhar copos", "Higiene das maos, etiqueta respiratoria e vacinacao", "Aglomerar", "Evitar a vacina"}, 1,
      "Medidas simples de higiene e vacinacao quebram a cadeia de transmissao." },
    { "Zoonoses sao doencas:",
      {"So de plantas", "Transmitidas entre animais e humanos", "So geneticas", "Causadas por poluicao sonora"}, 1,
      "Ex.: raiva e leptospirose; a vigilancia ambiental atua no controle." },
    { "A Atencao Primaria deve ser RESOLUTIVA, ou seja:",
      {"Encaminhar tudo ao hospital", "Resolver a maioria dos problemas no proprio territorio", "So marcar exames", "So dar atestado"}, 1,
      "Boa APS resolve ate ~80% das demandas de saude da populacao." },
    { "Educacao em saude na comunidade serve para:",
      {"Nada", "Empoderar as pessoas a cuidarem da propria saude e prevenir doencas", "Vender remedio", "Aumentar filas"}, 1,
      "Informacao e participacao melhoram desfechos de saude coletiva." },

    // ---- Microbiologia / conceitos do jogo (expansao 2 Mundos) ----
    { "O que sao BACTERIOFAGOS?",
      {"Antibioticos naturais", "Virus que infectam e destroem BACTERIAS", "Um tipo de vacina", "Celulas de defesa"}, 1,
      "Bacteriofagos sao virus que infectam bacterias — nao confundir com antibiotico." },
    { "Para que servem as VACINAS?",
      {"Curar qualquer doenca na hora", "Treinar o sistema imune a reconhecer o patogeno e prevenir doencas", "Substituir a higiene", "Matar bacterias resistentes"}, 1,
      "Vacinas preparam a imunidade contra doencas, sobretudo virais (ex.: influenza)." },
    { "O CAPSIDEO de um virus e:",
      {"O nucleo de RNA", "A capa proteica que protege o material genetico", "Um antibiotico", "Um tipo de celula"}, 1,
      "Neutralizar/romper o capsideo ajuda o sistema imune a conter o virus." },
    { "Dengue e influenza sao causadas por:",
      {"Bacterias", "Virus de RNA", "Fungos", "Protozoarios"}, 1,
      "Sao virus de RNA; a dengue e transmitida pelo Aedes aegypti." }
};

#define QUIZ_COUNT ((int)(sizeof(quizDB) / sizeof(quizDB[0])))
#define HISTORY_SIZE 14   // evita repetir as ~14 perguntas mais recentes na run

static int currentQuestionIdx = 0;
static int history[HISTORY_SIZE];
static int historyCount = 0;
static bool questionAnswered = false;
static int selectedOption = -1;
static bool quizInitialized = false;

static UIButton quizOptionsBtn[4];

static bool InHistory(int idx)
{
    for (int i = 0; i < historyCount; i++)
        if (history[i] == idx) return true;
    return false;
}

static void PushHistory(int idx)
{
    if (historyCount < HISTORY_SIZE) history[historyCount++] = idx;
    else { for (int i = 1; i < HISTORY_SIZE; i++) history[i - 1] = history[i]; history[HISTORY_SIZE - 1] = idx; }
}

void DrawTelaQuiz(GameState *game, Font font)
{
    if (!quizInitialized) {
        // Sorteia uma pergunta que NAO esteja no historico recente (baixa repeticao)
        int tries = 0;
        do {
            currentQuestionIdx = GetRandomValue(0, QUIZ_COUNT - 1);
            tries++;
        } while (InHistory(currentQuestionIdx) && tries < 200 && historyCount < QUIZ_COUNT);
        PushHistory(currentQuestionIdx);
        questionAnswered = false;
        selectedOption = -1;

        for (int i = 0; i < 4; i++) {
            quizOptionsBtn[i] = (UIButton){ { 240, 290 + i * 64, 800, 48 }, quizDB[currentQuestionIdx].options[i], false, false };
        }
        quizInitialized = true;
    }

    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, THEME_COLOR_BG_DARK, THEME_COLOR_BG_LIGHT);

    const char *title = "QUIZ DA VIGILANCIA EPIDEMIOLOGICA / SUS";
    Vector2 titleSz = MeasureTextEx(font, title, 30.0f, 1.0f);
    DrawTextEx(font, title, (Vector2){ (SCREEN_WIDTH / 2.0f) - (titleSz.x / 2.0f), 70.0f }, 30.0f, 1.0f, THEME_COLOR_MAIN);

    const char *question = quizDB[currentQuestionIdx].question;
    Vector2 qSz = MeasureTextEx(font, question, 24.0f, 1.0f);
    DrawTextEx(font, question, (Vector2){ (SCREEN_WIDTH / 2.0f) - (qSz.x / 2.0f), 175.0f }, 24.0f, 1.0f, WHITE);

    for (int i = 0; i < 4; i++) {
        Color baseCol = THEME_COLOR_PANEL;
        Color borderCol = THEME_COLOR_BORDER;
        Color textCol = WHITE;

        if (questionAnswered) {
            if (i == quizDB[currentQuestionIdx].correctOption) {
                baseCol = Fade(GREEN, 0.4f); borderCol = GREEN; textCol = GREEN;
            } else if (i == selectedOption) {
                baseCol = Fade(RED, 0.4f); borderCol = RED; textCol = RED;
            }
        } else if (quizOptionsBtn[i].hover) {
            baseCol = Fade(THEME_COLOR_BORDER, 0.8f); borderCol = THEME_COLOR_MAIN; textCol = THEME_COLOR_MAIN;
        }

        DrawRectangleRounded(quizOptionsBtn[i].bounds, 0.2f, 6, baseCol);
        DrawRectangleRoundedLines(quizOptionsBtn[i].bounds, 0.2f, 6, borderCol);
        DrawTextEx(font, TextFormat("%c) %s", 'A' + i, quizOptionsBtn[i].text),
                   (Vector2){ quizOptionsBtn[i].bounds.x + 20, quizOptionsBtn[i].bounds.y + 14 }, 20.0f, 1.0f, textCol);
    }

    if (questionAnswered) {
        bool correct = (selectedOption == quizDB[currentQuestionIdx].correctOption);
        const char *msg = correct ? "CORRETO! +50 PONTOS DO SUS" : "INCORRETO!";
        Color msgCol = correct ? GREEN : RED;
        Vector2 msgSz = MeasureTextEx(font, msg, 26.0f, 1.0f);
        DrawTextEx(font, msg, (Vector2){ (SCREEN_WIDTH / 2.0f) - (msgSz.x / 2.0f), 560.0f }, 26.0f, 1.0f, msgCol);

        // Explicacao curta da resposta correta (educativo)
        const char *exp = quizDB[currentQuestionIdx].explanation;
        Vector2 eSz = MeasureTextEx(font, exp, 17.0f, 1.0f);
        DrawTextEx(font, exp, (Vector2){ (SCREEN_WIDTH / 2.0f) - (eSz.x / 2.0f), 598.0f }, 17.0f, 1.0f, Fade(WHITE, 0.85f));

        const char *cont = "Clique em qualquer lugar para continuar...";
        Vector2 contSz = MeasureTextEx(font, cont, 16.0f, 1.0f);
        DrawTextEx(font, cont, (Vector2){ (SCREEN_WIDTH / 2.0f) - (contSz.x / 2.0f), 650.0f }, 16.0f, 1.0f, GRAY);
    }
}

void UpdateTelaQuiz(GameState *game, Vector2 mouse)
{
    if (questionAnswered) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            quizInitialized = false;
            game->currentScreen = SCREEN_UPGRADE; // segue para a tela de upgrades
        }
        return;
    }

    for (int i = 0; i < 4; i++) {
        if (CheckCollisionPointRec(mouse, quizOptionsBtn[i].bounds)) {
            quizOptionsBtn[i].hover = true;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                selectedOption = i;
                questionAnswered = true;
                if (selectedOption == quizDB[currentQuestionIdx].correctOption) {
                    game->player.susPoints += 50;
                }
            }
        } else {
            quizOptionsBtn[i].hover = false;
        }
    }
}
