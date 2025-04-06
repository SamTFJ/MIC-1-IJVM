#include "ULA.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring> // usar strcpy, strncpy (manipular char)
#include <bitset> // converte entre string binaria e inteiro
using namespace std;

//iniciando 
ULA::ULA() {
    strcpy(A, "11111111111111111111111111111111");
    strcpy(B, "00000000000000000000000000000001");
    strcpy(S, "00000000000000000000000000000000");
    strcpy(IR, "000000");
    PC = 1;
    carryout = 0;
}

// copia ate 32 caracteres de a.c_str() para A[]
void ULA::seta(string a){
    strncpy(A, a.c_str(), 32); //a.c_str() retorna um ponteiro de char, para o strncpy poder copiar
    A[32] = '\0';  //garante o '\0'
}

// mesma coisa para B[]
void ULA::setb(string b){
    strncpy(B, b.c_str(), 32);
    B[32] = '\0';  
}

int ULA::processar(bool F0, bool F1, bool ENA, bool ENB, bool INVA, bool INC) {
    //conjunto de 32 bits, recebe um const char*, cada caractere de A e B é interpretado como bit
    bitset<32> bsA(A); // bsA contem o padrão binario de A, logo bsa tem um array de 32 bits. A[0] (mais signficativo) A[31] (menos significativo)
    bitset<32> bsB(B); 

    // analisa se ENA e ENB estão habilitados
    unsigned int opA = ENA ? bsA.to_ulong() : 0; //Caso ENA esteja habilitado, pega os 32 bits e os interpreta como um inteiro, caso não esteja habilitado, será 0.
    unsigned int opB = ENB ? bsB.to_ulong() : 0;
    
    // Se INVA estiver ativo, inverte os bits de opA
    if (INVA) opA = (~opA) & 0xFFFFFFFF;
    
    
    // Converte F0 e F1 para um inteiro entre 0 e 3
    int op = (F0 ? 1 : 0) * 2 + (F1 ? 1 : 0);

    unsigned long long result = 0;

        //as 4 operações básicas da ULA
        
        switch(op) {
        case 0:  // 00: AND
            result = opA & opB;
            break;
        case 1:  // 01: OR
            result = opA | opB;
            break;
        case 2:  // 10: originalmente subtração, agora ADD (soma simples)
            result = (unsigned long long)opA + opB;
            break;
        case 3:  // 11: originalmente ADD (+ INC), agora soma com incremento (soma + (INC ? 1 : 0))
            result = (unsigned long long)opA + opB + (INC ? 1 : 0);
            break;
    }
          
    
    // Detecta carryout se o resultado exceder 32 bits
    carryout = (result > 0xFFFFFFFFu) ? 1 : 0;
    
    // result pode conter ate 64 bits, essa função considera apenas os 32 bits menos significativos
    unsigned int res32 = (unsigned int)(result & 0xFFFFFFFFu);
    
    // Converte o resultado para string binária e armazena em S
    string binRes = bitset<32>(res32).to_string();
    strncpy(S, binRes.c_str(), 32);
    S[32] = '\0';
    
    return res32;
}

void ULA::executar(string nomearquivo){
    ifstream arquivo(nomearquivo);
    ofstream arqlog("log.txt");

    //erro ao abrir
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo de instruções!" << endl;
        return;
    }
    
    // Cabeçalho do log 
    arqlog << "b = " << B << "\n";
    arqlog << "a = " << A << "\n\n";
    arqlog << "Programa Iniciado\n";
    arqlog << "============================================================\n";

   
    string linha;

    while (getline(arquivo, linha)) {

        //se a linha tiver vazia, irá encerrar
        if (linha.empty()){
            PC++;
            arqlog << "\nPC = " << PC << "\n> Line is empty, EOP.\n";
            break;
        }

        // 1) Carrega IR (6 bits)
        for (int i = 0; i < 6 && i < (int)linha.length(); ++i){
            IR[i] = linha[i];
        }
        IR[6] = '\0';

        // 2) Decodifica os sinais de controle a partir da linha
        F0   = (linha[0] == '1');
        F1   = (linha[1] == '1');
        ENA  = (linha[2] == '1');
        ENB  = (linha[3] == '1');
        INVA = (linha[4] == '1');
        INC  = (linha[5] == '1');

        // 3) Executa a operação da ULA
        int res = processar(F0, F1, ENA, ENB, INVA, INC);
        



        // 4) Grava no log
        arqlog << "\nCiclo " << PC << "\n\n";
        arqlog << "PC = " << PC << "\n";
        arqlog << "IR = " << IR << "\n";
        arqlog << "b = " << B << "\n";
        arqlog << "a = " << A << "\n";
        arqlog << "s = " << S << "\n";
        arqlog << "co = " << carryout << "\n";
        arqlog << "============================================================\n";


        PC++; //incrementa pc, indicando q vai para prox instrução
    }
    // fecha os arquivos
    arqlog.close();
    arquivo.close();
}

