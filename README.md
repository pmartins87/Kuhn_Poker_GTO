Solver de GTO para Kuhn Poker com CFR+ e Aproximação por Rede Neural
Visão Geral do Projeto
Este repositório contém uma implementação completa de uma pipeline de Inteligência Artificial para poker, demonstrada através da solução do jogo Kuhn Poker. O projeto é dividido em duas partes principais:

Motor GTO em C++: Um programa de alta performance escrito em C++ que utiliza o algoritmo CFR+ (Counterfactual Regret Minimization Plus) para resolver o Kuhn Poker. Através de milhões de iterações de auto-jogo, este motor converge para uma estratégia GTO (Game Theory Optimal), que é matematicamente inexplorável.

Aproximador em Python: Um conjunto de scripts em Python que utiliza TensorFlow/Keras para treinar uma Rede Neural. A rede neural aprende a imitar a estratégia GTO complexa gerada pelo motor C++, funcionando como um "cérebro" leve e rápido que pode tomar decisões GTO em tempo real.

Este projeto serve como uma "prova de conceito" e um laboratório de estudos para a arquitetura CFR -> Dataset -> NN, que é a base para a criação de bots de poker de nível mundial.

Conceitos Fundamentais
Kuhn Poker: É um jogo de poker simplificado (baralho de 3 cartas: Valete, Dama, Rei), amplamente utilizado em pesquisas acadêmicas para testar e validar algoritmos de Teoria dos Jogos.

GTO (Game Theory Optimal): Uma estratégia que não pode ser explorada por nenhum oponente, não importa como ele jogue. É baseada no conceito de Equilíbrio de Nash. O objetivo de um jogador GTO não é explorar as fraquezas de um oponente específico, mas sim jogar de uma forma tão sólida que o lucro surja naturalmente dos erros que os oponentes cometem ao desviar do GTO.

CFR+ (Counterfactual Regret Minimization Plus): É o algoritmo de aprendizado por reforço utilizado no motor C++. Ele funciona simulando o jogo contra si mesmo milhões de vezes e minimizando o "arrependimento" de não ter tomado ações melhores no passado. A variante "Plus" (CFR+) garante uma convergência mais rápida e estável em comparação com o CFR original.

Aproximação de Função (Function Approximation): O motor CFR+ é preciso, mas lento e pesado. A Rede Neural atua como um "aproximador de função": ela aprende a mapear uma situação de jogo (sua carta, histórico de apostas) para a estratégia GTO correta, sem precisar fazer os cálculos complexos do CFR. Ela aprende a "intuição" do GTO.

Arquitetura do Projeto
O fluxo de trabalho do projeto é o seguinte:

Motor CFR+ (C++) -> Gera -> kuhn_poker_gto.csv (Dataset) -> Treina -> Rede Neural (Python) -> Salva -> kuhn_poker_nn.keras (Modelo Treinado) -> Valida -> Simulação (Python)

Estrutura dos Arquivos
cfr_poker.h: O arquivo de cabeçalho do C++. Define a arquitetura do projeto, incluindo as structs Node e Card, e a classe principal CFRTrainer. É a "planta" do nosso motor.

cfr_poker.cpp: O arquivo-fonte do C++. Contém a implementação completa de toda a lógica: o algoritmo CFR+, as regras do Kuhn Poker, o cálculo de payoffs e as funções para salvar os resultados.

main.cpp: O ponto de entrada do programa C++. Orquestra o treinamento do motor CFR e, ao final, chama a função para salvar o dataset GTO no arquivo kuhn_poker_gto.csv.

kuhn_poker_gto.csv: Um arquivo de texto gerado pelo programa C++. Ele contém a estratégia GTO para cada situação de decisão única no Kuhn Poker, servindo como a "verdade absoluta" para o treinamento da rede neural.

train_nn.py: Script em Python que carrega o kuhn_poker_gto.csv, constrói uma rede neural com TensorFlow/Keras, e a treina para aprender a estratégia GTO.

simulate_match.py: Script em Python para validar a performance da rede neural treinada. Ele carrega o modelo salvo (kuhn_poker_nn.keras) e o coloca para jogar 1 milhão de mãos contra um bot "benchmark" que joga a GTO teórica, medindo a performance e provando que a estratégia aprendida é robusta.

Como Usar
Pré-requisitos
Um compilador C++ (g++, MSVC, etc.)

Python 3.x

As bibliotecas Python: tensorflow, pandas, scikit-learn, numpy

Bash

pip install tensorflow pandas scikit-learn numpy
Passo 1: Gerar a Estratégia GTO (C++)
Compile o projeto C++. Exemplo com g++:

Bash

g++ main.cpp cfr_poker.cpp -o kuhn_solver -std=c++17 -O3
Execute o programa compilado. Ele irá rodar as iterações do CFR+ (o padrão no main.cpp é 10 milhões).

Bash

./kuhn_solver
Ao final, o programa irá gerar o arquivo kuhn_poker_gto.csv.

Passo 2: Treinar a Rede Neural (Python)
Execute o script de treinamento.

Bash

python train_nn.py
O script irá carregar o CSV, construir e treinar o modelo. Ao final, ele salvará o modelo treinado como kuhn_poker_nn.keras e mostrará a precisão do aprendizado.

Passo 3: Validar a Rede Neural (Python)
Execute o script de simulação.

Bash

python simulate_match.py
O script irá carregar o modelo salvo, jogar 1 milhão de mãos contra um bot GTO teórico e apresentar a winrate final.

Resultado Esperado
Após o treinamento do CFR+, a saída do main.cpp mostrará a estratégia GTO aprendida, que deve ser próxima de:

Estrategia com um Valete (J) (no 0/):
  - PASS: ~80%
  - BET:  ~20%

Estrategia com uma Dama (Q) (no 1/):
  - PASS: ~100%
  - BET:  ~0%

Estrategia com um Rei (K) (no 2/):
  - PASS: ~35%
  - BET:  ~65%
Após rodar a simulação com simulate_match.py, o resultado esperado é:

Utilidade media da Rede Neural: ~0.00xxxxxx
Winrate da Rede Neural: ~X.XXXX milli-unidades/mao

Resultado: EMPATE TECNICO. A nossa Rede Neural joga uma estrategia GTO robusta e inexploravel!
Próximos Passos
Este projeto serve como a fundação validada. Os próximos passos no desenvolvimento do bot de poker final envolvem escalar esta arquitetura para o jogo de Texas Hold'em (Flop, Turn e River), expandir o conjunto de açõe.
