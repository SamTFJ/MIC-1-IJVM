#ifndef ULA_H
#define ULA_H
#include <iostream>
#include <string>
using namespace std;

class ULA {
    public:
        bool SLL8, SRA1, F0, F1, ENA, ENB, INVA, INC;
        char A[33];  // 32 bits + terminador '\0'
        char B[33];
        char IR[9];  // 21 bits + terminador '\0'
        char S[33]; 
        char Sd[33]; 
        int PCU;
        int carryout;
        int N;
        int Z;

        ULA();

        
        void executar(string nomearquivo);
        int processar(bool SLL8, bool SRA1, bool F0, bool F1, bool ENA, bool ENB, bool INVA, bool INC);
        void seta(string a);
        void setb(string b);
};

#endif