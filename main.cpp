// Arquivo: main.cpp

#include "cfr_poker.h"
#include <iostream>
#include <string> // Necess�rio para std::string

// --- FUN��O AUXILIAR NOVA ---
// Traduz o ID num�rico da carta para um nome leg�vel.
std::string getCardName(int cardValue) {
    switch (cardValue) {
    case 0: return "Valete (J)";
    case 1: return "Dama (Q)";
    case 2: return "Rei (K)";
    default: return "Carta Desconhecida";
    }
}

int main() {
    // Para um teste r�pido, 10 milh�es de itera��es � �timo.
    int iterations = 10000000;

    CFRTrainer trainer;
    trainer.train(iterations);

    std::cout << "\n--- ESTRATEGIAS FINAIS ---" << std::endl;

    // --- CHAMADAS ATUALIZADAS ---
    // Estrat�gia com a pior m�o
    std::cout << "\nEstrategia com um " << getCardName(0) << " (no 0/):" << std::endl;
    trainer.printStrategy("0/");

    // Estrat�gia com a m�o m�dia
    std::cout << "\nEstrategia com uma " << getCardName(1) << " (no 1/):" << std::endl;
    trainer.printStrategy("1/");

    // Estrat�gia com a melhor m�o
    std::cout << "\nEstrategia com um " << getCardName(2) << " (no 2/):" << std::endl;
    trainer.printStrategy("2/");

    std::cout << "\nPressione Enter para sair." << std::endl;
    std::cin.get();

    return 0;
}
// --- FIM DO C�DIGO ---