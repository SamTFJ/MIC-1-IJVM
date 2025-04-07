#ifndef ULA_H
#define ULA_H
#include <iostream>
#include <string>
using namespace std;

class ULA {
    public:
        bool F0, F1, ENA, ENB, INVA, INC;
        char A[33];  // 32 bits + terminador '\0'
        char B[33];
        char IR[7];  // 6 bits + terminador '\0'
        char S[33];  
        int PC;
        int carryout;

        ULA();

        bool linhaVazia(const string& linha);
        void executar(string nomearquivo);
        int processar(bool F0, bool F1, bool ENA, bool ENB, bool INVA, bool INC);
        void seta(string a);
        void setb(string b);
};

#endif