#pragma once // Substitui os antigos #ifndef, #define, e #endif
// Arquivo: cfr_poker.h

#include <vector>
#include <string>
#include <map>
#include <algorithm> // Necess�rio para std::shuffle
#include <random>    // Necess�rio para std::mt19937

// Definindo o n�mero de jogadores.
const int NUM_PLAYERS = 2;

// Enum para as a��es poss�veis.
enum Action {
    PASS,
    BET
};

// A estrutura principal que representa um n� na �rvore do jogo.
struct Node {
    // --- Identificador do N� (Chave �nica) ---
    std::string infoSet;

    // --- Estado do Jogo ---
    int playerToAct;

    // --- Estrat�gia e Arrependimento (O Cora��o do CFR) ---
    int numActions;
    std::vector<double> regretSum;
    std::vector<double> strategySum;

    // Construtor Corrigido: Usado para inicializar um novo n�.
    Node(int actions, int player, const std::string& set)
        : numActions(actions), playerToAct(player), infoSet(set) {
        regretSum.assign(numActions, 0.0);
        strategySum.assign(numActions, 0.0);
    }

    // --- M�todos (Fun��es do N�) ---

    // Calcula a estrat�gia atual com base nos arrependimentos.
    std::vector<double> getStrategy() {
        std::vector<double> strategy(numActions);
        double totalRegret = 0;

        for (int i = 0; i < numActions; ++i) {
            strategy[i] = regretSum[i] > 0 ? regretSum[i] : 0;
            totalRegret += strategy[i];
        }

        if (totalRegret > 0) {
            for (int i = 0; i < numActions; ++i) {
                strategy[i] /= totalRegret;
            }
        }
        else {
            for (int i = 0; i < numActions; ++i) {
                strategy[i] = 1.0 / numActions;
            }
        }

        return strategy;
    }

    // Calcula a estrat�gia m�dia, que � a nossa estrat�gia GTO final.
    std::vector<double> getAverageStrategy() {
        std::vector<double> avgStrategy(numActions);
        double totalSum = 0;

        for (int i = 0; i < numActions; ++i) {
            totalSum += strategySum[i];
        }

        if (totalSum > 0) {
            for (int i = 0; i < numActions; ++i) {
                avgStrategy[i] = strategySum[i] / totalSum;
            }
        }
        else {
            for (int i = 0; i < numActions; ++i) {
                avgStrategy[i] = 1.0 / numActions;
            }
        }

        return avgStrategy;
    }
};

// A classe principal que orquestra o treinamento.
class CFRTrainer {
public:
    CFRTrainer();
    void train(int iterations);
    void printStrategy(const std::string& infoSet);

private:
    std::map<std::string, Node> nodeMap;
    std::vector<int> deck;

    double cfr(const std::vector<int>& cards, const std::string& history, double p0, double p1);

    // Fun��es auxiliares
    std::string cardsToString(const std::vector<int>& cards);
    std::string historyToString(const std::vector<Action>& history);
};

/*
Explica��o do C�digo
Node struct: � o nosso "molde" para cada ponto de decis�o.

infoSet: Esta ser� uma string �nica que funciona como uma "impress�o digital" do n� (ex: "cartas do hero/cartas do board/hist�rico de apostas"). Usaremos um std::map<std::string, Node> para guardar todos os n�s que encontrarmos, usando essa string como a chave. Isso evita ter que construir a �rvore inteira na mem�ria de uma vez.

playerToAct: Simplesmente nos diz se � a vez do jogador 0 ou do jogador 1.

regretSum: Este � o cora��o do CFR. � um vetor (array) que armazena o "arrependimento" acumulado para cada a��o poss�vel. Se regretSum[0] for muito alto, significa que o algoritmo se arrepende muito de n�o ter tomado a A��o 0 no passado.

strategySum: Para obter a estrat�gia GTO final, n�o usamos a estrat�gia de uma �nica itera��o, mas sim a m�dia de todas as estrat�gias ao longo do tempo. Este vetor acumula essas probabilidades.

getStrategy(): Esta fun��o implementa a regra principal do CFR: a probabilidade de escolher uma a��o � proporcional ao seu arrependimento positivo. Se uma a��o n�o tem arrependimento positivo, ela n�o � escolhida (a menos que nenhuma a��o tenha arrependimento, nesse caso, joga-se de forma aleat�ria).

getAverageStrategy(): Simplesmente calcula a m�dia das estrat�gias salvas em strategySum para nos dar nosso resultado final.
*/

/*
Explica��o da Nova Classe
CFRTrainer(): O construtor. Sua principal tarefa ser� criar e embaralhar o baralho no in�cio de cada m�o.

train(int iterations): Esta ser� a �nica fun��o "p�blica" que chamaremos de fora da classe. Ela ter� um loop simples que rodar� o processo de treinamento pelo n�mero de itera��es que definirmos. Ex: for (int i = 0; i < iterations; i++) { ... }.

nodeMap: Como mencionei antes, este � o nosso "dicion�rio" de n�s. Quando chegarmos a um ponto do jogo, criaremos a string (infoSet) que o representa e verificaremos se ele j� existe no nodeMap. Se n�o, criamos um novo Node e o adicionamos. Se sim, usamos o que j� est� l�. Isso � muito mais eficiente do que gerar a �rvore inteira.

deck: Um simples vetor de inteiros (std::vector<int>) que representa nosso baralho de 52 cartas.

cfr(...): Esta � a fun��o mais importante e complexa que escreveremos.

� recursiva: ela chama a si mesma para explorar os galhos da �rvore de decis�o.

cards: Guardar� as cartas dos jogadores e do board.

history: A sequ�ncia de a��es que nos trouxe at� este ponto.

p0, p1: Par�metros cruciais do CFR chamados "reach probabilities" (probabilidades de alcance). p0 � a probabilidade de que o jogador 0 jogue de uma forma que chegue a este n�. p1 � a mesma coisa para o jogador 1. Isso � essencial para ponderar corretamente os arrependimentos. N�o se preocupe, vamos detalhar isso quando formos implementar a fun��o.
*/
