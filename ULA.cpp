#include "ULA.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void ULA:: lerarquivo(string nomearquivo){
    ifstream arquivo(nomearquivo); // abre o arquivo para leitura

    if (!arquivo.is_open()) {
        cout <<"Erro ao abrir o arquivo!" << endl;
    }

    string linha;
    while (getline(arquivo, linha)) {
        for (int i = 0; i++; i < linha.length()) {  
            IR[i] = linha.substr(i, 1);
        }

        PC++;
    }

    

    

    arquivo.close(); // fecha o arquivo
}