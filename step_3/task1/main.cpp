#include <iostream>
#include "ULA.h"
#include "REGISTRADOR.H"

using namespace std;

int main() {
    ULA ula;
    ula.executar("programa_etapa3_tarefa1.txt");
    cout << "Execução concluída. Verifique o arquivo log.txt para os resultados." << endl;
    
    return 0;
}