#ifndef FRONTEND_HPP
#define FRONTEND_HPP

#include <iostream>
#include <vector>
#include <regex>

using namespace std;

struct EntradaTabla {
    int iSimbolo;
    string sSimbolo;
    char tipo;
    int ubicacion;
    EntradaTabla* sig;
};

struct EstructuraMemoria {
    int ubicacion;
    EstructuraMemoria* sig;
};

class Frontend {
    public: //metodos
        Frontend(string);
        ~Frontend();
        
    public: //atributos
        string nFichero;
        vector<string> vLinea;
        string eRem;
        string eInput;
        string ePrint;
        string eEnd;
        string eLineaBlanco;
        string eIfGoto;
        string eLet;
        string eGoto;

        EntradaTabla* tablaSimbolo;
        EstructuraMemoria* memoria;
        int contadorInstrucciones;
        int contadorVariables;

    protected: // metodos
         void primeraPasada();
    
    private: // metodos
         void abrirFichero();
         void analizaEstructuraLineas();
         void analizadorLexico(string&);
         void analizadorSintactico();

         void agregarElementoTablaSimbolo(const string&, const char);
         void imprimirTablaSimbolo();

         void vaciarMemoria();
         void vaciarTablaSimbolo();
};

#endif //FRONTEND_HPP