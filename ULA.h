#ifndef ULA_H
#define ULA_H
#include <iostream>
#include <string>
using namespace std;

class ULA {
    public:
        bool F0, F1, ENA, ENB, INVA, INC, A, B;
        string IR[6];
        int PC;
        int S;
        int carryout;

        void executar(string arquivo);

        int processar(bool F0, bool F1, bool ENA, bool ENB, bool INVA, bool INC, bool A, bool B);
        

    ULA(){
        A = 1;
        B = 1;
    }
};

#endif