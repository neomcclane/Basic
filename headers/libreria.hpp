#ifndef LIBRERIA_HPP
#define LIBRERIA_HPP

#include <iostream>
#include <exception>
#include <stdexcept>

using namespace std;

namespace lib {
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

    // funciones
    void validarFichero(string&);

}

#endif //LIBRERIA_HPP