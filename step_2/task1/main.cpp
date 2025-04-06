#include <iostream>
#include "ULA.h"

using namespace std;

int main() {
    ULA ula;
    ula.executar("programa_etapa2.txt");
    cout << "Execução concluída. Verifique o arquivo log.txt para os resultados." << endl;
    
    return 0;
}