#ifndef COMPILADOR_HPP
#define COMPILADOR_HPP

#include <iostream>
#include <vector>
#include "frontend.hpp"
#include "backend.hpp"

using namespace std;

class Compilador:public Frontend, public Backend {
    public: //metodos
        Compilador(string);
        ~Compilador();
        void run();
    
    public: //atributos
        string nFichero();
};

#endif //COMPILADOR_HPP