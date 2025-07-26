// Arquivo: cfr_poker.cpp

#include "cfr_poker.h"
#include <iostream>
#include <string> // Adicionado para std::to_string

// Implementação do Construtor
CFRTrainer::CFRTrainer() {
    // Inicializa o baralho com 52 cartas
    for (int i = 0; i < 52; ++i) {
        deck.push_back(i);
    }
}

// Implementação da Função de Treinamento
void CFRTrainer::train(int iterations) {
    double utility = 0.0;

    // O "baralho" do Kuhn Poker tem apenas 3 cartas. Vamos usar 0, 1, 2 para J, Q, K.
    std::vector<int> deck = { 0, 1, 2 };

    std::random_device rd;
    std::mt19937 g(rd());

    std::cout << "Iniciando treinamento (Kuhn Poker) por " << iterations << " iteracoes..." << std::endl;

    for (int i = 0; i < iterations; ++i) {
        // Embaralha nosso baralho de 3 cartas.
        std::shuffle(deck.begin(), deck.end(), g);

        // Distribui as cartas para os 2 jogadores.
        std::vector<int> cards;
        cards.push_back(deck[0]); // Carta do Jogador 0
        cards.push_back(deck[1]); // Carta do Jogador 1

        // O resto da lógica continua igual!
        utility += cfr(cards, "", 1.0, 1.0);

        if ((i + 1) % 100000 == 0) { // Aumentei o intervalo do log
            std::cout << "Iteracao: " << (i + 1) << ", Utilidade Media: " << utility / (i + 1) << std::endl;
        }
    }

    std::cout << "Treinamento concluido." << std::endl;
    std::cout << "Utilidade Media Final: " << utility / iterations << std::endl;
}

// Implementação da função CFR principal
double CFRTrainer::cfr(const std::vector<int>& cards, const std::string& history, double p0, double p1) {

    int player_to_act = history.length() % 2;
    int opponent = 1 - player_to_act;

    // =================================================================
    // PARTE 1: CASOS BASE (NÓS TERMINAIS)
    // =================================================================
    if (history.length() >= 2) {
        int payoff_for_p0 = 0; // Usamos 0 como indicador de que não é um nó terminal ainda

        if (history == "pp") {      // Check, Check -> Showdown (pote baixo)
            payoff_for_p0 = (cards[0] > cards[1]) ? 1 : -1;
        }
        else if (history == "bb") {  // Bet, Call -> Showdown (pote alto)
            payoff_for_p0 = (cards[0] > cards[1]) ? 2 : -2;
        }
        else if (history == "pbb") { // Pass, Bet, Call -> Showdown (pote alto)
            payoff_for_p0 = (cards[0] > cards[1]) ? 2 : -2;
        }
        else if (history == "bp") {  // Bet, Fold
            payoff_for_p0 = 1;
        }
        else if (history == "pbp") { // Pass, Bet, Fold
            payoff_for_p0 = -1;
        }

        // Se payoff_for_p0 foi alterado, encontramos um nó terminal.
        if (payoff_for_p0 != 0) {
            return (player_to_act == 0) ? payoff_for_p0 : -payoff_for_p0;
        }
    }

    // =================================================================
    // PARTE 2: NÓS NÃO-TERMINAIS (PASSO RECURSIVO)
    // =================================================================

    std::string infoSet = std::to_string(cards[player_to_act]) + "/" + history;

    auto it = nodeMap.find(infoSet);
    if (it == nodeMap.end()) {
        it = nodeMap.emplace(infoSet, Node(2, player_to_act, infoSet)).first;
    }
    Node* node = &it->second;

    std::vector<double> strategy = node->getStrategy();
    std::vector<double> actionUtils(node->numActions);
    double nodeUtility = 0;

    for (int i = 0; i < node->numActions; ++i) {
        std::string nextHistory = history + (i == 0 ? "p" : "b");

        if (player_to_act == 0) {
            actionUtils[i] = -cfr(cards, nextHistory, p0 * strategy[i], p1);
        }
        else {
            actionUtils[i] = -cfr(cards, nextHistory, p0, p1 * strategy[i]);
        }

        nodeUtility += strategy[i] * actionUtils[i];
    }

    for (int i = 0; i < node->numActions; ++i) {
        double regret = actionUtils[i] - nodeUtility;
        double reachProb = (player_to_act == 0) ? p1 : p0;
        node->regretSum[i] += reachProb * regret; // (CFR+)
        if (node->regretSum[i] < 0) {
            node->regretSum[i] = 0;
        }

        double myReachProb = (player_to_act == 0) ? p0 : p1;
        node->strategySum[i] += myReachProb * strategy[i];
    }

    return nodeUtility;
}

// Implementação "stub" (vazia) das funções auxiliares
std::string CFRTrainer::cardsToString(const std::vector<int>& cards) {
    return ""; // TODO
}

std::string CFRTrainer::historyToString(const std::vector<Action>& history) {
    return ""; // TODO
}

void CFRTrainer::printStrategy(const std::string& infoSet) {
    auto it = nodeMap.find(infoSet);
    if (it == nodeMap.end()) {
        // Se a estratégia não for encontrada, podemos imprimir uma mensagem ou ficar em silêncio.
        // Vamos manter a mensagem de erro por enquanto.
        std::cout << "  -> Estrategia para o no " << infoSet << " nao encontrada." << std::endl;
        return;
    }

    Node* node = &it->second;
    std::vector<double> avgStrategy = node->getAverageStrategy();

    // Agora a função apenas imprime os dados, não o título.
    std::cout << "  - PASS (Check/Fold): " << avgStrategy[0] * 100 << "%" << std::endl;
    std::cout << "  - BET (Bet/Call):    " << avgStrategy[1] * 100 << "%" << std::endl;
}
/*
Explicação do Código
Construtor CFRTrainer::CFRTrainer()
Este código é executado uma vez quando criamos um objeto CFRTrainer.

Ele simplesmente preenche nosso vetor deck com 52 inteiros, de 0 a 51. Cada número representa uma carta única. Essa é uma forma computacionalmente eficiente de lidar com cartas.

Função CFRTrainer::train(int iterations)
Esta é a função que chamaremos para iniciar todo o processo.

utility: Usamos esta variável para somar os resultados de cada mão. Ao dividir pelo número de iterações, podemos ver o EV (Valor Esperado) médio do jogo. Em um jogo simétrico, esse valor deve tender a zero, indicando que nosso algoritmo está convergindo corretamente.

Embaralhamento: Em vez do antigo std::random_shuffle, usamos o std::shuffle com um gerador mt19937 do C++11, que fornece uma aleatoriedade de muito maior qualidade, essencial para simulações como esta.

Distribuição de Cartas: Por enquanto, para manter a simplicidade absoluta, estamos simulando um jogo super simples onde cada jogador recebe apenas uma carta. Isso nos permite focar 100% na lógica do CFR. Assim que validarmos que o CFR funciona, expandiremos para 2 cartas por jogador e adicionaremos o board.

Chamada do cfr(...): Este é o ponto de partida para a recursão em cada mão.

cards: Passamos as cartas que acabamos de distribuir.

"": O histórico de ações começa vazio na raiz da árvore.

1.0, 1.0: No início da mão, a probabilidade de cada jogador ter chegado a este ponto é 100% (ou 1.0). Essas são as "reach probabilities" iniciais.

Progresso: O if ((i + 1) % 1000 == 0) é apenas uma forma de nos dar feedback visual durante um treinamento longo, imprimindo o status a cada 1000 mãos simuladas.
*/


/*
EXCELENTE! Fico muito feliz em ouvir isso. Ter um projeto dessa complexidade compilando sem erros é o marco mais importante. Isso significa que nossa fundação está sólida.

Agora, a parte mais empolgante. Vamos adicionar a peça final do quebra-cabeça: o loop recursivo e o mecanismo de aprendizado da função cfr.

Fase 1, Etapa 6: O Coração do CFR - Recursão e Arrependimento
Até agora, nossa função cfr sabe:

Identificar o fim da mão e calcular o resultado (nós terminais).

Para uma decisão no meio da mão, ela sabe encontrar ou criar o nó e obter a estratégia atual.

O que faremos agora é substituir o return 0.0; provisório pela lógica que explora cada ação possível, descobre seus resultados e aprende com eles, atualizando os arrependimentos.

Sua Tarefa: Substitua sua função CFRTrainer::cfr inteira pela versão completa abaixo.

C++

// Arquivo: cfr_poker.cpp
// Substitua sua função cfr() inteira por esta versão completa.

double CFRTrainer::cfr(const std::vector<int>& cards, const std::string& history, double p0, double p1) {

    int player_to_act = history.length() % 2;
    int opponent = 1 - player_to_act;

    // =================================================================
    // PARTE 1: CASOS BASE (NÓS TERMINAIS) - JÁ IMPLEMENTADO
    // =================================================================
    if (history.length() >= 2) {
        char last_action = history.back();
        char second_last_action = history[history.length() - 2];

        bool showdown = (last_action == 'b' && second_last_action == 'b') || // Bet, Call
                        (last_action == 'b' && second_last_action == 'p') || // Check, Bet, Call
                        (history == "pp");                                 // Check, Check

        bool fold = (last_action == 'p' && second_last_action == 'b');   // Bet, Fold

        if (showdown) {
            int payoff = (cards[0] > cards[1]) ? 1 : -1;
            // Se houve uma aposta, o valor em jogo é maior
            if (history.find('b') != std::string::npos) {
                payoff *= 2;
            }
            return payoff;
        }
        if (fold) {
            // O jogador que apostou ganha a ante do oponente.
            return 1.0;
        }
    }

    // =================================================================
    // PARTE 2: NÓS NÃO-TERMINAIS (PASSO RECURSIVO) - LÓGICA COMPLETA
    // =================================================================

    // 1. Identificar o nó e obter a estratégia (já implementado)
    std::string infoSet = std::to_string(cards[player_to_act]) + "/" + history;
    auto it = nodeMap.find(infoSet);
    if (it == nodeMap.end()) {
        it = nodeMap.emplace(infoSet, Node(2, player_to_act, infoSet)).first;
    }
    Node* node = &it->second;
    std::vector<double> strategy = node->getStrategy();

    // 2. Iterar sobre as ações, explorar recursivamente e calcular utilidades
    std::vector<double> actionUtils(node->numActions);
    double nodeUtility = 0;

    for (int i = 0; i < node->numActions; ++i) {
        std::string nextHistory = history + (i == 0 ? "p" : "b"); // 'p' para PASS, 'b' para BET

        // A utilidade retornada pela chamada recursiva é do ponto de vista do PRÓXIMO jogador.
        // Portanto, invertemos o sinal para obter a utilidade do ponto de vista do JOGADOR ATUAL.
        if (player_to_act == 0) {
            actionUtils[i] = -cfr(cards, nextHistory, p0 * strategy[i], p1);
        } else {
            actionUtils[i] = -cfr(cards, nextHistory, p0, p1 * strategy[i]);
        }

        // Acumula a utilidade total do nó, ponderada pela estratégia.
        nodeUtility += strategy[i] * actionUtils[i];
    }

    // 3. Calcular e acumular arrependimentos e estratégia média
    for (int i = 0; i < node->numActions; ++i) {
        // Arrependimento = (Utilidade da ação) - (Utilidade média do nó)
        double regret = actionUtils[i] - nodeUtility;

        // Pondera o arrependimento e a estratégia pelas probabilidades de alcance (reach probabilities)
        double reachProb = (player_to_act == 0) ? p1 : p0; // Probabilidade do oponente chegar aqui
        node->regretSum[i] += reachProb * regret;

        double myReachProb = (player_to_act == 0) ? p0 : p1; // Probabilidade de eu chegar aqui
        node->strategySum[i] += myReachProb * strategy[i];
    }

    return nodeUtility;
}
Explicação da Lógica Adicionada
Esta é a parte mais densa, vamos quebrar:

actionUtils e nodeUtility: actionUtils é um vetor para guardar o valor (EV) de cada ação possível (PASS e BET). nodeUtility será o valor médio esperado deste nó, considerando nossa estratégia atual.

O Loop for: Nós iteramos sobre cada ação i (0 para PASS, 1 para BET).

A Chamada Recursiva -cfr(...):

- (Sinal Negativo): Este é um detalhe crucial. A função cfr retorna a utilidade do ponto de vista do jogador da vez. Quando chamamos cfr para o próximo estado, o jogador da vez será nosso oponente. Portanto, a utilidade retornada será a dele. Para obter a nossa utilidade, simplesmente invertemos o sinal.

p0 * strategy[i]: Aqui atualizamos a "probabilidade de alcance". Se a chance de o Jogador 0 chegar a este nó era p0, e a chance dele escolher a ação i é strategy[i], então a chance de ele chegar ao próximo nó (após a ação i) é p0 * strategy[i].

Cálculo da Utilidade do Nó: Após o loop, nodeUtility é calculado como uma média ponderada. É o EV total deste nó se seguirmos a estratégia atual. (Probabilidade de PASS * EV de PASS) + (Probabilidade de BET * EV de BET).

Cálculo e Acumulação do Arrependimento:

regret = actionUtils[i] - nodeUtility: Esta é a fórmula mágica. O arrependimento de uma ação é o quão melhor (ou pior) ela foi em comparação com o valor médio de ter chegado a este nó.

node->regretSum[i] += reachProb * regret: Nós somamos o arrependimento ao nosso total acumulado, mas o ponderamos pela probabilidade do oponente ter chegado a este nó. Isso garante que damos mais peso aos arrependimentos em partes da árvore que são alcançadas com mais frequência.

Acumulação da Estratégia Média: De forma similar, acumulamos a probabilidade da estratégia (strategy[i]) ponderada pela nossa própria probabilidade de alcance. Isso é o que nos dará a estratégia GTO final no getAverageStrategy().
*/