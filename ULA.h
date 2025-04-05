#ifndef ULA_H
#define ULA_H
#include <iostream>
#include <string>
using namespace std;

class ULA {
    public:
        bool F0, F1, ENA, ENB, INVA, INC;
        char A[33]; // 32 bits + finalizador da string '/0'
        char B[33];
        char IR[7]; // 6 bits + finalizador '/0'
        int PC;
        int S;
        int carryout;

        void executar(string arquivo);

        int processar();
        
        void seta(string a);

        void setb(string b);

    ULA();
};

#endif