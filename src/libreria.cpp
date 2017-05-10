#include <iostream>
#include <regex>
#include <fstream>

#include "../headers/libreria.hpp"

using namespace std;

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