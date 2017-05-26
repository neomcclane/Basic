#include <iostream>
#include <regex>
#include <fstream>

#include "../headers/libreria.hpp"

using namespace std;

int lib::EstructuraMemoria::tamano() {
    int contador = 0;

    EstructuraMemoria* auxMemoria = this;
    while(auxMemoria != nullptr) {
        auxMemoria = auxMemoria->sig;
        contador++;
    }

    return contador;
}

int lib::Pila::tamano() {
    int contador = 0;
    Pila* auxMemoria = this;

    while(auxMemoria != nullptr) {
        auxMemoria = auxMemoria->sig;
        contador++;
    }
    return contador;
}

void lib::validarFichero(string& nFichero) {

    regex e("(.*)(\\.)(bsl)");

    if(!regex_match(nFichero, e)) {
        throw lib::ENombreFichero("Error, nombre de fichero invalido.");
    }
    else {
        ifstream fichero;
        fichero.open(nFichero);

        if(!fichero.is_open()) {
            fichero.close();
            throw lib::EFicheroNoExiste("Error, el fichero no existe.");
        }
        fichero.close();
    }
}