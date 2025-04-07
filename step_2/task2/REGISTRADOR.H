#ifndef REGISTRADORES_H
#define REGISTRADORES_H
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

class Registradores{
    public:
    char MAR [33];
    char MDR [33];
    char PC [33];
    char MBR [9];
    char SP [33];
    char LV [33];
    char CPP [33];
    char TOS [33];
    char OPC [33];
    char H [33];

    Registradores();

    string decod4(uint8_t code4); 

    void escritaC(uint16_t regc, char Sd[33]);
    

};



#endif