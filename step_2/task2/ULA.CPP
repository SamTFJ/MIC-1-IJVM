#include "ULA.h"
#include "REGISTRADOR.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring> // usar strcpy, strncpy (manipular char)
#include <bitset> // converte entre string binaria e inteiro
using namespace std;

//iniciando registradores
Registradores::Registradores(){
    strcpy( MAR, "00000000000000000000000000000000");
    strcpy( MDR, "00000000000000000000000000000000");
    strcpy( PC, "00000000000000000000000000000000");
    strcpy( MBR, "10000001");
    strcpy( SP, "00000000000000000000000000000000");
    strcpy( LV, "00000000000000000000000000000000");
    strcpy( CPP, "00000000000000000000000000000000");
    strcpy( TOS, "00000000000000000000000000000000");
    strcpy( OPC, "00000000000000000000000000000000");
    strcpy( H, "00000000000000000000000000000000");
    
 } 


// decodificador de 4 bits para hssbilitar um dos registradores a comandar o barramento B
string Registradores:: decod4(uint8_t code4) {
    switch (code4){
    case 8:
        return string(OPC);
    case 7:
        return string(TOS);
    case 6:
        return string(CPP);
    case 5:
        return string(LV);
    case 4:
        return string(SP);
    case 3:{
        string MBRU = string(24,'0') + string(MBR); // extendendo MBR de 8 para 32 bits
        return MBRU;
    }
    case 2:{
        char sinal = MBR[0];
        string MBREX = string(24,sinal) + string(MBR);
        return MBREX;
    }
    case 1:
        return string(PC);
    case 0:
        return string(MDR);
        
    }

    return string (32, '0'); 

}


void Registradores::escritaC(uint16_t regc, char Sd[33]){
    if (regc & (1<<8)) strcpy(H,   Sd); //se o bit correspondente em regc estiver em 1, copia a string de Sd para o registrador
    if (regc & (1<<7)) strcpy(OPC, Sd);
    if (regc & (1<<6)) strcpy(TOS, Sd);
    if (regc & (1<<5)) strcpy(CPP, Sd);
    if (regc & (1<<4)) strcpy(LV,  Sd);
    if (regc & (1<<3)) strcpy(SP,  Sd);
    if (regc & (1<<2)) strcpy(PC,  Sd);
    if (regc & (1<<1)) strcpy(MDR, Sd);
    if (regc & (1<<0)) strcpy(MAR, Sd);
}

// objeto global de registradores para poder usar na ULA
Registradores regs;


//iniciando ULA
ULA::ULA() {
    strcpy(A, "00000000000000000000000000000000");
    strcpy(B, "00000000000000000000000000000000");
    strcpy(S, "00000000000000000000000000000000");
    strcpy(Sd,"00000000000000000000000000000000");
    strcpy(IR, "00000000");
    PC = 1;
    N = 0;
    Z = 0;
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

int ULA::processar(bool SLL8, bool SRA1,bool F0, bool F1, bool ENA, bool ENB, bool INVA, bool INC) {
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


    if(SLL8 & !SRA1){

        res32 = res32 << 8; // deslocamento lógico para esquerda em 8 bits

    } else if(SRA1 & !SLL8){
       int32_t  resultadosra1 = static_cast<int32_t>(res32); //var res32(unsigned int) é convertida para um inteiro com sinal, a converção é forçada pelo static_cast<int_32t>

        resultadosra1 = resultadosra1 >>1; // deslocamento aritmético para direita em 1 bit

        res32 = static_cast<unsigned int>(resultadosra1); //var res32 é convertida de volta para unsingned int
    }

    string binresd = bitset<32>(res32).to_string();
    strncpy(Sd, binresd.c_str(),32);
    Sd[32] = '\0';

    if(res32 & 0x80000000u){
        
        N = 1;
    } 
    else{
        N = 0;
    }

    if(res32 == 0){
        Z = 1;
    }
    else{
        Z = 0;
    }
    
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

    arqlog << "============================================================\n\n";
    arqlog << "Estado Inicial dos Registradores:\n";
    arqlog << "H   = "   << regs.H   << "\n";
    arqlog << "OPC = "   << regs.OPC << "\n";
    arqlog << "TOS = "   << regs.TOS << "\n";
    arqlog << "CPP = "   << regs.CPP << "\n";
    arqlog << "LV  = "   << regs.LV  << "\n";
    arqlog << "SP  = "   << regs.SP  << "\n";
    arqlog << "PC  = "   << regs.PC  << "\n";
    arqlog << "MDR = "   << regs.MDR << "\n";
    arqlog << "MAR = "   << regs.MAR << "\n";
    arqlog << "MBR = "   << regs.MBR << "\n";
    arqlog << "============================================================\n\n";
   
    string linha;

    while (getline(arquivo, linha)) {

        //se a linha tiver vazia, irá encerrar
        if (linha.empty()){
            PC++;
            arqlog << "\nPC = " << PC << "\n> Line is empty, EOP.\n";
            break;
        }

        if(linha.size() < 21){
            arqlog << "Instruções incompletas" << endl;
            continue;
        }

        // como a instrução completa agr tem 21 bits(8 da ULA, 9 do C, 4 do B.) Agora temos:
        string ULAcontrole = linha.substr(0,8); //pega os 8 primeitos bits
        string barramentoC = linha.substr(8,9); //pega os 9 bits seguintes
        string barramentoB = linha.substr(17,4); //pega os 4 bits finais

        //atualiza IR da ULA com os 8 primeiros bits de controle
        strncpy(IR, ULAcontrole.c_str(), 8);
        IR[8] = '\0';

        // Decodifica os sinais de controle a partir da linha
        SLL8 = (linha[0] == '1');
        SRA1 = (linha[1] == '1');
        F0   = (linha[2] == '1');
        F1   = (linha[3] == '1');
        ENA  = (linha[4] == '1');
        ENB  = (linha[5] == '1');
        INVA = (linha[6] == '1');
        INC  = (linha[7] == '1');
       
        seta(string(regs.H)); // configura a entrada A com o conteudo do registrador H

        int code4 = stoi(barramentoB, nullptr,2); //stoi consulta qual registrador sera ativo. nullptr pra nao pegar o resto da string, 2 indica base binaria
        string valorB = regs.decod4(static_cast<uint8_t>(code4));//pega o código de 0 a 8 e retorna o conteudo do registrador correspondente
        setb(valorB); //configura a entrada B com o registrador ativo no momento

        // Executa a operação da ULA
        int res = processar(SLL8, SRA1, F0, F1, ENA, ENB, INVA, INC);

        uint16_t regc = stoi(barramentoC, nullptr, 2);
        regs.escritaC(regc, Sd);


        //  verifica se SLL8 = SLLA = 1, quando isso ocorre, o sinal de controle é inválido
        if(SLL8 & SRA1){
            arqlog << "\nCiclo " << PC << "\n\n";
            arqlog << "PC = " << PC << "\n";
            arqlog << "IR = " << IR << "\n";
            arqlog << ">Erro! Sinais de Controle inválidos."<< endl;  
            arqlog << "============================================================\n";
            return;
        }

        // Grava no log
        arqlog << "\nCiclo " << PC << "\n\n";
        arqlog << "ir = " << IR << barramentoC << barramentoB <<  "\n";
        arqlog << "Entrada A (H)         = " << regs.H << "\n";
        arqlog << "Entrada B             = " << B << "\n";
        arqlog << "Saída da ULA (S)      = " << S << "\n";
        arqlog << "Saída Deslocada (Sd)  = " << Sd << "\n";
        arqlog << "N = " << N << "\n";
        arqlog << "Z = " << Z << "\n";
        arqlog << "Carryout = " << carryout << "\n\n";
        arqlog << "============================================================\n";


        PC++; //incrementa pc, indicando q vai para prox instrução
    }
    // fecha os arquivos
    arqlog.close();
    arquivo.close();
}

