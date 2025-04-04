#ifndef ULA_H
#define ULA_H
#include <iostream>
#include <string>
using namespace std;

class ULA {
    public:
        bool F0, F1, ENA, ENB, INVA, INC;
        bool A, B;
        string IR;
        int PC;
        int S;

        void lerarquivo(string arquivo);

    ULA(){
        A = 1;
        B = 1;
    }
};

#endif