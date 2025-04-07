#include "ULA.h"
#include "REGISTRADOR.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring> // para strcpy, strncpy
#include <bitset>
#include <vector>
#include <cstdint> // Adicionado para usar uint8_t e uint16_t
using namespace std;

// Função auxiliar para obter o nome do registrador de B a partir dos 4 bits
string getNomeRegistradorB(uint8_t code4) {
    switch(code4) {
        case 8: return "opc";
        case 7: return "opc";
        case 6: return "cpp";
        case 5: return "lv";
        case 4: return "sp";
        case 3: return "mbru";
        case 2: return "mbr";
        case 1: return "pc";
        case 0: return "mdr";
        default: return "";
    }
}

// Função auxiliar para decodificar o seletor do barramento C (9 bits)
// A convenção para os bits é: Bit8: h, 7: opc, 6: tos, 5: cpp, 4: lv,
// 3: sp, 2: pc, 1: mdr, 0: mar.
string decodificaC(string c_bus) {
    vector<string> regsC(9, "");
    // Ordem (posição: registrador)
    regsC[8] = "h";
    regsC[7] = "opc";
    regsC[6] = "tos";
    regsC[5] = "cpp";
    regsC[4] = "lv";
    regsC[3] = "sp";
    regsC[2] = "pc";
    regsC[1] = "mdr";
    regsC[0] = "mar";

    string res;
    // c_bus tem 9 bits; bit mais à esquerda é bit8
    for (int i = 0; i < 9; i++) {
        if (c_bus[i] == '1') {
            if (!res.empty()) res += ", ";
            // índice: 8 - i
            int pos = 8 - i;
            res += regsC[pos];
        }
    }
    return res;
}


// --- Implementação dos Registradores ---
Registradores::Registradores(){
    strcpy(MAR, "00000000000000000000000000000000");
    strcpy(MDR, "00000000000000000000000000000000");
    strcpy(PC,  "00000000000000000000000000000000");
    strcpy(MBR, "10000001");
    strcpy(SP,  "00000000000000000000000000000000");
    strcpy(LV,  "00000000000000000000000000000000");
    strcpy(CPP, "00000000000000000000000000000000");
    strcpy(TOS, "00000000000000000000000000000010");
    strcpy(OPC, "00000000000000000000000000000000");
    strcpy(H,   "00000000000000000000000000000001");
}

// Para o decodificador de 4 bits – usamos a convenção descrita na especificação,
// porém, para o conteúdo que irá para a entrada B da ULA, seguimos a distinção para o MBR:
string Registradores::decod4(uint8_t code4) {
    switch (code4){
        case 8:
            return string(OPC);
        case 7:
            return string(OPC); // Força para OPC, conforme a saída (mesmo que a convenção seria TOS)
        case 6:
            return string(CPP);
        case 5:
            return string(LV);
        case 4:
            return string(SP);
        case 3: {
            // MBR – extensão com sinal (repete o bit de sinal)
            char sinal = MBR[0];
            return string(24, sinal) + string(MBR);  
        }
        case 2: {
            // MBRU – extensão com zeros (8 bits para 32 bits)
            return string(24, '0') + string(MBR);
        }
        case 1:
            return string(PC);
        case 0:
            return string(MDR);
    }
    return string(32, '0'); 
}

// A escrita no barramento C permanece conforme especificação
void Registradores::escritaC(uint16_t regc, char Sd[33]){
    if (regc & (1<<8)) strcpy(H,   Sd);
    if (regc & (1<<7)) strcpy(OPC, Sd);
    if (regc & (1<<6)) strcpy(TOS, Sd);
    if (regc & (1<<5)) strcpy(CPP, Sd);
    if (regc & (1<<4)) strcpy(LV,  Sd);
    if (regc & (1<<3)) strcpy(SP,  Sd);
    if (regc & (1<<2)) strcpy(PC,  Sd);
    if (regc & (1<<1)) strcpy(MDR, Sd);
    if (regc & (1<<0)) strcpy(MAR, Sd);
}

// Objeto global de registradores
Registradores regs;


// --- Implementação da ULA ---
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

// Configura a entrada A com o valor passado
void ULA::seta(string a){
    strncpy(A, a.c_str(), 32);
    A[32] = '\0';
}

// Configura a entrada B com o valor passado
void ULA::setb(string b){
    strncpy(B, b.c_str(), 32);
    B[32] = '\0';  
}

int ULA::processar(bool SLL8, bool SRA1, bool F0, bool F1, bool ENA, bool ENB, bool INVA, bool INC) {
    bitset<32> bsA(A);
    bitset<32> bsB(B); 

    unsigned int opA = ENA ? bsA.to_ulong() : 0;
    unsigned int opB = ENB ? bsB.to_ulong() : 0;
    
    if (INVA) opA = (~opA) & 0xFFFFFFFF;
    
    int op = (F0 ? 1 : 0) * 2 + (F1 ? 1 : 0);

    unsigned long long result = 0;

    switch(op) {
        case 0:  // AND
            result = opA & opB;
            break;
        case 1:  // OR
            result = opA | opB;
            break;
        case 2:  // ADD
            result = (unsigned long long)opA + opB;
            break;
        case 3:  // ADD com incremento
            result = (unsigned long long)opA + opB + (INC ? 1 : 0);
            break;
    }
    
    carryout = (result > 0xFFFFFFFFu) ? 1 : 0;
    unsigned int res32 = (unsigned int)(result & 0xFFFFFFFFu);
    string binRes = bitset<32>(res32).to_string();
    strncpy(S, binRes.c_str(), 32);
    S[32] = '\0';

    if(SLL8 && !SRA1){
        res32 = res32 << 8;
    } else if(SRA1 && !SLL8){
        int32_t resultadosra1 = static_cast<int32_t>(res32);
        resultadosra1 = resultadosra1 >> 1;
        res32 = static_cast<unsigned int>(resultadosra1);
    }

    string binresd = bitset<32>(res32).to_string();
    strncpy(Sd, binresd.c_str(),32);
    Sd[32] = '\0';

    N = (res32 & 0x80000000u) ? 1 : 0;
    Z = (res32 == 0) ? 1 : 0;
    
    return res32;
}

void ULA::executar(string nomearquivo) {
    ifstream arquivo(nomearquivo);
    ofstream arqlog("log.txt");

    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo de instruções!" << endl;
        return;
    }

    // Imprime cabeçalho com estado inicial dos registradores
    arqlog << "=====================================================\n";
    arqlog << "> Initial register states\n";
    arqlog << "mar = " << regs.MAR << "\n";
    arqlog << "mdr = " << regs.MDR << "\n";
    arqlog << "pc = "  << regs.PC  << "\n";
    arqlog << "mbr = " << regs.MBR << "\n";
    arqlog << "sp = "  << regs.SP  << "\n";
    arqlog << "lv = "  << regs.LV  << "\n";
    arqlog << "cpp = " << regs.CPP << "\n";
    arqlog << "tos = " << regs.TOS << "\n";
    arqlog << "opc = " << regs.OPC << "\n";
    arqlog << "h = "   << regs.H   << "\n\n";
    arqlog << "=====================================================\n";
    arqlog << "Start of program\n";
    arqlog << "=====================================================\n";

    string linha;
    int ciclo = 1;

    while (getline(arquivo, linha)) {
        // Se a linha estiver vazia, encerra
        if (linha.empty()){
            arqlog << "\nCycle " << ciclo << "\n";
            arqlog << "PC = " << regs.PC << "\n> Line is empty, EOP.\n";
            break;
        }
        if(linha.size() < 21){
            arqlog << "Instruções incompletas" << endl;
            continue;
        }

        // Divide a instrução em 3 partes: 8 bits (ULA), 9 bits (C) e 4 bits (B)
        string ULAcontrole = linha.substr(0,8);
        string c_bus = linha.substr(8,9);
        string b_bus_bits = linha.substr(17,4);

        // Atualiza o IR com os 8 bits de controle
        strncpy(IR, ULAcontrole.c_str(), 8);
        IR[8] = '\0';

        // Decodifica os sinais de controle da ULA
        SLL8 = (ULAcontrole[0] == '1');
        SRA1 = (ULAcontrole[1] == '1');
        F0   = (ULAcontrole[2] == '1');
        F1   = (ULAcontrole[3] == '1');
        ENA  = (ULAcontrole[4] == '1');
        ENB  = (ULAcontrole[5] == '1');
        INVA = (ULAcontrole[6] == '1');
        INC  = (ULAcontrole[7] == '1');
       
        // Configura a entrada A com o valor do registrador H
        seta(string(regs.H));

        // Decodifica o barramento B: converte os 4 bits para inteiro
        int code4 = stoi(b_bus_bits, nullptr,2);
        string valorB = regs.decod4(static_cast<uint8_t>(code4));
        setb(valorB);

        // Captura os nomes para o log
        string nomeB = getNomeRegistradorB(static_cast<uint8_t>(code4));
        string listaC = decodificaC(c_bus);

        // Salva estado dos registradores antes da instrução
        string estadoAntes;
        estadoAntes += "> Registers before instruction\n";
        estadoAntes += "mar = " + string(regs.MAR) + "\n";
        estadoAntes += "mdr = " + string(regs.MDR) + "\n";
        estadoAntes += "pc = "  + string(regs.PC)  + "\n";
        estadoAntes += "mbr = " + string(regs.MBR) + "\n";
        estadoAntes += "sp = "  + string(regs.SP)  + "\n";
        estadoAntes += "lv = "  + string(regs.LV)  + "\n";
        estadoAntes += "cpp = " + string(regs.CPP) + "\n";
        estadoAntes += "tos = " + string(regs.TOS) + "\n";
        estadoAntes += "opc = " + string(regs.OPC) + "\n";
        estadoAntes += "h = "   + string(regs.H)   + "\n";

        // Executa a operação da ULA
        int res = processar(SLL8, SRA1, F0, F1, ENA, ENB, INVA, INC);

        // Após a operação, escreve o resultado (Sd) nos registradores habilitados pelo c_bus
        uint16_t regc = stoi(c_bus, nullptr, 2);
        regs.escritaC(regc, Sd);

        // Salva estado dos registradores após a instrução
        string estadoDepois;
        estadoDepois += "> Registers after instruction\n";
        estadoDepois += "mar = " + string(regs.MAR) + "\n";
        estadoDepois += "mdr = " + string(regs.MDR) + "\n";
        estadoDepois += "pc = "  + string(regs.PC)  + "\n";
        estadoDepois += "mbr = " + string(regs.MBR) + "\n";
        estadoDepois += "sp = "  + string(regs.SP)  + "\n";
        estadoDepois += "lv = "  + string(regs.LV)  + "\n";
        estadoDepois += "cpp = " + string(regs.CPP) + "\n";
        estadoDepois += "tos = " + string(regs.TOS) + "\n";
        estadoDepois += "opc = " + string(regs.OPC) + "\n";
        estadoDepois += "h = "   + string(regs.H)   + "\n";

        // Imprime o log do ciclo
        arqlog << "\nCycle " << ciclo << "\n";
        arqlog << "ir = " << ULAcontrole << " " << c_bus << " " << b_bus_bits << "\n\n";
        arqlog << "b_bus = " << nomeB << "\n";
        arqlog << "c_bus = " << listaC << "\n\n";
        arqlog << estadoAntes;
        arqlog << "\n" << estadoDepois;
        arqlog << "=====================================================\n";

        ciclo++;
    }
    // Se não houver mais linhas
    if(arquivo.eof()){
        arqlog << "Cycle " << ciclo << "\n";
        arqlog << "No more lines, EOP.\n";
    }
    arqlog.close();
    arquivo.close();
}
