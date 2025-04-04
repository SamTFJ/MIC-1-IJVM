#include "ULA.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void ULA:: lerarquivo(string nomearquivo){
    ifstream arquivo(nomearquivo); // abre o arquivo para leitura

    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo!" << std::endl;
    }

    string linha;
    while (std::getline(arquivo, linha)) {
        IR = linha;
        PC = PC +1;
    }

    

    

    arquivo.close(); // fecha o arquivo
}