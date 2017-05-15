#include <iostream>
#include "../headers/compilador.hpp"
#include "../headers/frontend.hpp"

using namespace std;

Compilador::Compilador(string nFichero):Frontend(nFichero) {

}

Compilador::~Compilador() {

}

void Compilador::run() {
    this->primeraPasada();
    this->segundaPasada();
}