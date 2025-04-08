#include <iostream>
#include "ULA.h"
#include "REGISTRADOR.H"

using namespace std;

int main() {
    ULA ula;
    ula.processarArquivoInstrucoes("instruções.txt");
    ula.executar("instruções.txt");
    cout << "Execução concluída. Verifique o arquivo log.txt para os resultados." << endl;
    
    return 0;
}