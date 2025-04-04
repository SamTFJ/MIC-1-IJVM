#include "ULA.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;



void ULA:: executar(string nomearquivo){
    ifstream arquivo(nomearquivo); // abre o arquivo para leitura

    ofstream arqlog("log.txt"); // Abre o arquivo de log
        if (!arqlog.is_open()) {
            std::cerr << "Não foi possível criar o arquivo!" << std::endl;
        }

    if (!arquivo.is_open()) { // Verifica se o arquivo foi aberto corretamente
        cout <<"Erro ao abrir o arquivo!" << endl;
    }

    string linha;
    while (getline(arquivo, linha)) { // Lê cada linha do arquivo e separa em caracteres
        for (int i = 0; i++; i < linha.length()) {  
            IR[i] = linha.substr(i, 1);
        }

        // Associa cada caracter a uma entrada
        F0 = linha[0];
        F1 = linha[1];
        ENA = linha[2];
        ENB = linha[3];
        INVA = linha[4];
        INC = linha[5];

       
        
        // Acrescenta +1 no PC, sinalizando que a operação foi concluida e vai para a próxima instrução
        PC++;
    }

    

    

    arquivo.close(); // fecha o arquivo
}