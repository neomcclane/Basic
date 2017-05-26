#ifndef BACKEND_HPP
#define BACKEND_HPP

#include "libreria.hpp"

class Backend {
    public:
        // metodos
        Backend();
        ~Backend();

        //atributos
        int acumulador;
        int contadorInstrucciones;
        int registroInstruccion;
        int codigoOperacion;
        int operando;
        
    protected:
        void analizarMemoria(lib::EstructuraMemoria*);
};

#endif //BACKEND_HPP