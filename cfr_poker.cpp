// Arquivo: cfr_poker.cpp

#include "cfr_poker.h"
#include <iostream>
#include <string> // Adicionado para std::to_string

// Implementa��o do Construtor
CFRTrainer::CFRTrainer() {
    // Inicializa o baralho com 52 cartas
    for (int i = 0; i < 52; ++i) {
        deck.push_back(i);
    }
}

// Implementa��o da Fun��o de Treinamento
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

        // O resto da l�gica continua igual!
        utility += cfr(cards, "", 1.0, 1.0);

        if ((i + 1) % 100000 == 0) { // Aumentei o intervalo do log
            std::cout << "Iteracao: " << (i + 1) << ", Utilidade Media: " << utility / (i + 1) << std::endl;
        }
    }

    std::cout << "Treinamento concluido." << std::endl;
    std::cout << "Utilidade Media Final: " << utility / iterations << std::endl;
}

// Implementa��o da fun��o CFR principal
double CFRTrainer::cfr(const std::vector<int>& cards, const std::string& history, double p0, double p1) {

    int player_to_act = history.length() % 2;
    int opponent = 1 - player_to_act;

    // =================================================================
    // PARTE 1: CASOS BASE (N�S TERMINAIS)
    // =================================================================
    if (history.length() >= 2) {
        int payoff_for_p0 = 0; // Usamos 0 como indicador de que n�o � um n� terminal ainda

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

        // Se payoff_for_p0 foi alterado, encontramos um n� terminal.
        if (payoff_for_p0 != 0) {
            return (player_to_act == 0) ? payoff_for_p0 : -payoff_for_p0;
        }
    }

    // =================================================================
    // PARTE 2: N�S N�O-TERMINAIS (PASSO RECURSIVO)
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

// Implementa��o "stub" (vazia) das fun��es auxiliares
std::string CFRTrainer::cardsToString(const std::vector<int>& cards) {
    return ""; // TODO
}

std::string CFRTrainer::historyToString(const std::vector<Action>& history) {
    return ""; // TODO
}

void CFRTrainer::printStrategy(const std::string& infoSet) {
    auto it = nodeMap.find(infoSet);
    if (it == nodeMap.end()) {
        // Se a estrat�gia n�o for encontrada, podemos imprimir uma mensagem ou ficar em sil�ncio.
        // Vamos manter a mensagem de erro por enquanto.
        std::cout << "  -> Estrategia para o no " << infoSet << " nao encontrada." << std::endl;
        return;
    }

    Node* node = &it->second;
    std::vector<double> avgStrategy = node->getAverageStrategy();

    // Agora a fun��o apenas imprime os dados, n�o o t�tulo.
    std::cout << "  - PASS (Check/Fold): " << avgStrategy[0] * 100 << "%" << std::endl;
    std::cout << "  - BET (Bet/Call):    " << avgStrategy[1] * 100 << "%" << std::endl;
}
/*
Explica��o do C�digo
Construtor CFRTrainer::CFRTrainer()
Este c�digo � executado uma vez quando criamos um objeto CFRTrainer.

Ele simplesmente preenche nosso vetor deck com 52 inteiros, de 0 a 51. Cada n�mero representa uma carta �nica. Essa � uma forma computacionalmente eficiente de lidar com cartas.

Fun��o CFRTrainer::train(int iterations)
Esta � a fun��o que chamaremos para iniciar todo o processo.

utility: Usamos esta vari�vel para somar os resultados de cada m�o. Ao dividir pelo n�mero de itera��es, podemos ver o EV (Valor Esperado) m�dio do jogo. Em um jogo sim�trico, esse valor deve tender a zero, indicando que nosso algoritmo est� convergindo corretamente.

Embaralhamento: Em vez do antigo std::random_shuffle, usamos o std::shuffle com um gerador mt19937 do C++11, que fornece uma aleatoriedade de muito maior qualidade, essencial para simula��es como esta.

Distribui��o de Cartas: Por enquanto, para manter a simplicidade absoluta, estamos simulando um jogo super simples onde cada jogador recebe apenas uma carta. Isso nos permite focar 100% na l�gica do CFR. Assim que validarmos que o CFR funciona, expandiremos para 2 cartas por jogador e adicionaremos o board.

Chamada do cfr(...): Este � o ponto de partida para a recurs�o em cada m�o.

cards: Passamos as cartas que acabamos de distribuir.

"": O hist�rico de a��es come�a vazio na raiz da �rvore.

1.0, 1.0: No in�cio da m�o, a probabilidade de cada jogador ter chegado a este ponto � 100% (ou 1.0). Essas s�o as "reach probabilities" iniciais.

Progresso: O if ((i + 1) % 1000 == 0) � apenas uma forma de nos dar feedback visual durante um treinamento longo, imprimindo o status a cada 1000 m�os simuladas.
*/


/*
EXCELENTE! Fico muito feliz em ouvir isso. Ter um projeto dessa complexidade compilando sem erros � o marco mais importante. Isso significa que nossa funda��o est� s�lida.

Agora, a parte mais empolgante. Vamos adicionar a pe�a final do quebra-cabe�a: o loop recursivo e o mecanismo de aprendizado da fun��o cfr.

Fase 1, Etapa 6: O Cora��o do CFR - Recurs�o e Arrependimento
At� agora, nossa fun��o cfr sabe:

Identificar o fim da m�o e calcular o resultado (n�s terminais).

Para uma decis�o no meio da m�o, ela sabe encontrar ou criar o n� e obter a estrat�gia atual.

O que faremos agora � substituir o return 0.0; provis�rio pela l�gica que explora cada a��o poss�vel, descobre seus resultados e aprende com eles, atualizando os arrependimentos.

Sua Tarefa: Substitua sua fun��o CFRTrainer::cfr inteira pela vers�o completa abaixo.

C++

// Arquivo: cfr_poker.cpp
// Substitua sua fun��o cfr() inteira por esta vers�o completa.

double CFRTrainer::cfr(const std::vector<int>& cards, const std::string& history, double p0, double p1) {

    int player_to_act = history.length() % 2;
    int opponent = 1 - player_to_act;

    // =================================================================
    // PARTE 1: CASOS BASE (N�S TERMINAIS) - J� IMPLEMENTADO
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
            // Se houve uma aposta, o valor em jogo � maior
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
    // PARTE 2: N�S N�O-TERMINAIS (PASSO RECURSIVO) - L�GICA COMPLETA
    // =================================================================

    // 1. Identificar o n� e obter a estrat�gia (j� implementado)
    std::string infoSet = std::to_string(cards[player_to_act]) + "/" + history;
    auto it = nodeMap.find(infoSet);
    if (it == nodeMap.end()) {
        it = nodeMap.emplace(infoSet, Node(2, player_to_act, infoSet)).first;
    }
    Node* node = &it->second;
    std::vector<double> strategy = node->getStrategy();

    // 2. Iterar sobre as a��es, explorar recursivamente e calcular utilidades
    std::vector<double> actionUtils(node->numActions);
    double nodeUtility = 0;

    for (int i = 0; i < node->numActions; ++i) {
        std::string nextHistory = history + (i == 0 ? "p" : "b"); // 'p' para PASS, 'b' para BET

        // A utilidade retornada pela chamada recursiva � do ponto de vista do PR�XIMO jogador.
        // Portanto, invertemos o sinal para obter a utilidade do ponto de vista do JOGADOR ATUAL.
        if (player_to_act == 0) {
            actionUtils[i] = -cfr(cards, nextHistory, p0 * strategy[i], p1);
        } else {
            actionUtils[i] = -cfr(cards, nextHistory, p0, p1 * strategy[i]);
        }

        // Acumula a utilidade total do n�, ponderada pela estrat�gia.
        nodeUtility += strategy[i] * actionUtils[i];
    }

    // 3. Calcular e acumular arrependimentos e estrat�gia m�dia
    for (int i = 0; i < node->numActions; ++i) {
        // Arrependimento = (Utilidade da a��o) - (Utilidade m�dia do n�)
        double regret = actionUtils[i] - nodeUtility;

        // Pondera o arrependimento e a estrat�gia pelas probabilidades de alcance (reach probabilities)
        double reachProb = (player_to_act == 0) ? p1 : p0; // Probabilidade do oponente chegar aqui
        node->regretSum[i] += reachProb * regret;

        double myReachProb = (player_to_act == 0) ? p0 : p1; // Probabilidade de eu chegar aqui
        node->strategySum[i] += myReachProb * strategy[i];
    }

    return nodeUtility;
}
Explica��o da L�gica Adicionada
Esta � a parte mais densa, vamos quebrar:

actionUtils e nodeUtility: actionUtils � um vetor para guardar o valor (EV) de cada a��o poss�vel (PASS e BET). nodeUtility ser� o valor m�dio esperado deste n�, considerando nossa estrat�gia atual.

O Loop for: N�s iteramos sobre cada a��o i (0 para PASS, 1 para BET).

A Chamada Recursiva -cfr(...):

- (Sinal Negativo): Este � um detalhe crucial. A fun��o cfr retorna a utilidade do ponto de vista do jogador da vez. Quando chamamos cfr para o pr�ximo estado, o jogador da vez ser� nosso oponente. Portanto, a utilidade retornada ser� a dele. Para obter a nossa utilidade, simplesmente invertemos o sinal.

p0 * strategy[i]: Aqui atualizamos a "probabilidade de alcance". Se a chance de o Jogador 0 chegar a este n� era p0, e a chance dele escolher a a��o i � strategy[i], ent�o a chance de ele chegar ao pr�ximo n� (ap�s a a��o i) � p0 * strategy[i].

C�lculo da Utilidade do N�: Ap�s o loop, nodeUtility � calculado como uma m�dia ponderada. � o EV total deste n� se seguirmos a estrat�gia atual. (Probabilidade de PASS * EV de PASS) + (Probabilidade de BET * EV de BET).

C�lculo e Acumula��o do Arrependimento:

regret = actionUtils[i] - nodeUtility: Esta � a f�rmula m�gica. O arrependimento de uma a��o � o qu�o melhor (ou pior) ela foi em compara��o com o valor m�dio de ter chegado a este n�.

node->regretSum[i] += reachProb * regret: N�s somamos o arrependimento ao nosso total acumulado, mas o ponderamos pela probabilidade do oponente ter chegado a este n�. Isso garante que damos mais peso aos arrependimentos em partes da �rvore que s�o alcan�adas com mais frequ�ncia.

Acumula��o da Estrat�gia M�dia: De forma similar, acumulamos a probabilidade da estrat�gia (strategy[i]) ponderada pela nossa pr�pria probabilidade de alcance. Isso � o que nos dar� a estrat�gia GTO final no getAverageStrategy().
*/