#include <iostream>
#include "../headers/compilador.hpp"
#include "../headers/frontend.hpp"
#include "../headers/backend.hpp"

using namespace std;

Compilador::Compilador(string nFichero):Frontend(nFichero),Backend() {

}

Compilador::~Compilador() {

}

void Compilador::run() {
    this->primeraPasada();
    analizarMemoria(segundaPasada());
}