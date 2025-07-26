#pragma once // Substitui os antigos #ifndef, #define, e #endif
// Arquivo: cfr_poker.h

#include <vector>
#include <string>
#include <map>
#include <algorithm> // Necessário para std::shuffle
#include <random>    // Necessário para std::mt19937

// Definindo o número de jogadores.
const int NUM_PLAYERS = 2;

// Enum para as ações possíveis.
enum Action {
    PASS,
    BET
};

// A estrutura principal que representa um nó na árvore do jogo.
struct Node {
    // --- Identificador do Nó (Chave Única) ---
    std::string infoSet;

    // --- Estado do Jogo ---
    int playerToAct;

    // --- Estratégia e Arrependimento (O Coração do CFR) ---
    int numActions;
    std::vector<double> regretSum;
    std::vector<double> strategySum;

    // Construtor Corrigido: Usado para inicializar um novo nó.
    Node(int actions, int player, const std::string& set)
        : numActions(actions), playerToAct(player), infoSet(set) {
        regretSum.assign(numActions, 0.0);
        strategySum.assign(numActions, 0.0);
    }

    // --- Métodos (Funções do Nó) ---

    // Calcula a estratégia atual com base nos arrependimentos.
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

    // Calcula a estratégia média, que é a nossa estratégia GTO final.
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

    // Funções auxiliares
    std::string cardsToString(const std::vector<int>& cards);
    std::string historyToString(const std::vector<Action>& history);
};

/*
Explicação do Código
Node struct: É o nosso "molde" para cada ponto de decisão.

infoSet: Esta será uma string única que funciona como uma "impressão digital" do nó (ex: "cartas do hero/cartas do board/histórico de apostas"). Usaremos um std::map<std::string, Node> para guardar todos os nós que encontrarmos, usando essa string como a chave. Isso evita ter que construir a árvore inteira na memória de uma vez.

playerToAct: Simplesmente nos diz se é a vez do jogador 0 ou do jogador 1.

regretSum: Este é o coração do CFR. É um vetor (array) que armazena o "arrependimento" acumulado para cada ação possível. Se regretSum[0] for muito alto, significa que o algoritmo se arrepende muito de não ter tomado a Ação 0 no passado.

strategySum: Para obter a estratégia GTO final, não usamos a estratégia de uma única iteração, mas sim a média de todas as estratégias ao longo do tempo. Este vetor acumula essas probabilidades.

getStrategy(): Esta função implementa a regra principal do CFR: a probabilidade de escolher uma ação é proporcional ao seu arrependimento positivo. Se uma ação não tem arrependimento positivo, ela não é escolhida (a menos que nenhuma ação tenha arrependimento, nesse caso, joga-se de forma aleatória).

getAverageStrategy(): Simplesmente calcula a média das estratégias salvas em strategySum para nos dar nosso resultado final.
*/

/*
Explicação da Nova Classe
CFRTrainer(): O construtor. Sua principal tarefa será criar e embaralhar o baralho no início de cada mão.

train(int iterations): Esta será a única função "pública" que chamaremos de fora da classe. Ela terá um loop simples que rodará o processo de treinamento pelo número de iterações que definirmos. Ex: for (int i = 0; i < iterations; i++) { ... }.

nodeMap: Como mencionei antes, este é o nosso "dicionário" de nós. Quando chegarmos a um ponto do jogo, criaremos a string (infoSet) que o representa e verificaremos se ele já existe no nodeMap. Se não, criamos um novo Node e o adicionamos. Se sim, usamos o que já está lá. Isso é muito mais eficiente do que gerar a árvore inteira.

deck: Um simples vetor de inteiros (std::vector<int>) que representa nosso baralho de 52 cartas.

cfr(...): Esta é a função mais importante e complexa que escreveremos.

É recursiva: ela chama a si mesma para explorar os galhos da árvore de decisão.

cards: Guardará as cartas dos jogadores e do board.

history: A sequência de ações que nos trouxe até este ponto.

p0, p1: Parâmetros cruciais do CFR chamados "reach probabilities" (probabilidades de alcance). p0 é a probabilidade de que o jogador 0 jogue de uma forma que chegue a este nó. p1 é a mesma coisa para o jogador 1. Isso é essencial para ponderar corretamente os arrependimentos. Não se preocupe, vamos detalhar isso quando formos implementar a função.
*/
