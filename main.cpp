// Arquivo: main.cpp

#include "cfr_poker.h"
#include <iostream>
#include <string> // Necessário para std::string

// --- FUNÇÃO AUXILIAR NOVA ---
// Traduz o ID numérico da carta para um nome legível.
std::string getCardName(int cardValue) {
    switch (cardValue) {
    case 0: return "Valete (J)";
    case 1: return "Dama (Q)";
    case 2: return "Rei (K)";
    default: return "Carta Desconhecida";
    }
}

int main() {
    // Para um teste rápido, 10 milhões de iterações é ótimo.
    int iterations = 10000000;

    CFRTrainer trainer;
    trainer.train(iterations);

    std::cout << "\n--- ESTRATEGIAS FINAIS ---" << std::endl;

    // --- CHAMADAS ATUALIZADAS ---
    // Estratégia com a pior mão
    std::cout << "\nEstrategia com um " << getCardName(0) << " (no 0/):" << std::endl;
    trainer.printStrategy("0/");

    // Estratégia com a mão média
    std::cout << "\nEstrategia com uma " << getCardName(1) << " (no 1/):" << std::endl;
    trainer.printStrategy("1/");

    // Estratégia com a melhor mão
    std::cout << "\nEstrategia com um " << getCardName(2) << " (no 2/):" << std::endl;
    trainer.printStrategy("2/");

    std::cout << "\nPressione Enter para sair." << std::endl;
    std::cin.get();

    return 0;
}
// --- FIM DO CÓDIGO ---