#include "ULA.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

ULA::ULA(){
    PC = 0;
}

void ULA::seta(string a){
    for (int i = 0; i < 32 && i < a.size(); i++) {
        A[i] = a[i];
    }
}

void ULA::setb(string b){
    for (int i = 0; i < 32 && i < b.size(); i++) {
        B[i] = b[i];
    }
}


int ULA::processar() {
    // Define os valores de entrada
    int
    
     a = (ENA ? (INVA ? !A : A) : 0); // verifica se ENA esta habilitada, caso sim, verifica se o INVA esta habilitado
    int b = (ENB ? B : 0); // verifica se ENB esta habilitada
    
    int result = 0;
    int op = (F0 ? 1 : 0) * 2 + (F1 ? 1 : 0);  // transforma os bits em inteiro pra usar no switch case
    
    switch(op) {
        case 0:  // 00: AND, F0 = 0 e F1 = 0, então op = 0*2 + 0 = 0 (case0)
            result = (a & b);
            break;
        case 1:  // 01: subtração, F0 = 0 e F1 = 1, então op = 0*2 + 1 = 1 (case1)
        {
            result = a-b;
            break;
        }
        case 2:  // 10: OR, F0 = 1 e F1 = 0, então op = 1*2 + 0 = 2 (case2)
            result = (a | b);
            break;
        case 3:  // 11: soma, F0 = 1 e F2 = 1, então op = 1*2 + 1 = 3 (case3)
        {
            int sum = a + b + (INC ? 1 : 0); //soma e verifica se ocorre INC
            carryout = (sum >= 2) ? 1 : 0; 
            result = sum % 2;
            break;
        }
    }
    
    return result;
}

void ULA::executar(string nomearquivo){
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
        for (int i = 0; i < 6 && i < linha.length(); i++) {
            IR[i] = linha[i];
        }

        // Associa cada caracter a uma entrada
        F0 = (IR[0] == '1');
        F1 = (IR[1] == '1');
        ENA = (IR[2] == '1');
        ENB = (IR[3] == '1');
        INVA = (IR[4] == '1');
        INC = (IR[5] == '1');

        S = processar();

        //escreve no arquivo log
        arqlog << "PC: " << PC 
        << " | IR: " << linha 
        << " | A: " << A 
        << " | B: " << B 
        << " | S: " << S 
        << " | CO: " << carryout << "\n";
        
        // Acrescenta +1 no PC, sinalizando que a operação foi concluida e vai para a próxima instrução
        PC++;
    }
    
    arqlog.close();  // fecha o arquivo log
    arquivo.close(); // fecha o arquivo
}