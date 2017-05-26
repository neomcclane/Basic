#ifndef LIBRERIA_HPP
#define LIBRERIA_HPP

#include <iostream>
#include <exception>
#include <stdexcept>

using namespace std;

namespace lib {
    // estructuras
    struct EntradaTabla {
        int iSimbolo;
        string sSimbolo;
        char tipo;
        int ubicacion;
        EntradaTabla* sig;
    };

    struct EstructuraMemoria {
        int ubicacion;
        int instruccion;
        EstructuraMemoria* sig;

        int tamano();
    };

    struct Pila {
        string simbolo;
        bool estado;
        struct Pila* sig; 
        struct Pila* ant;
        int tamano();
    };

    // excepciones
    class ENombreFichero: public runtime_error {
        public:
            ENombreFichero(string msg): runtime_error(msg){}
    };

    class EFicheroNoExiste: public runtime_error {
        public:
            EFicheroNoExiste(string msg): runtime_error(msg) {}
    };

    class ELexico: public runtime_error {
        public:
            ELexico(string msg): runtime_error(msg) {}
    };

    class EVariableNoDeclarada: public runtime_error {

        public:
            EVariableNoDeclarada(string msg):runtime_error(msg) {}

    };

    class ELineaRepetida: public runtime_error {
        public:
            ELineaRepetida(string msg):runtime_error(msg) {}
    };

    class EErrorUbicacionTS: public runtime_error {
        public:
            EErrorUbicacionTS(string msg): runtime_error(msg){}
    };

    // funciones
    void validarFichero(string&);

}

#endif //LIBRERIA_HPP