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
    int instruccion;
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
        int multiplicador;

    protected: // metodos
         void primeraPasada();
         void segundaPasada();
    
    private: // metodos
         void abrirFichero();
         void analizaEstructuraLineas();
         void analizadorLexico(string&);
         void analizadorSintactico();

         void agregarElementoTablaSimbolo(const string&, const char);
         void imprimirTablaSimbolo();
         bool existeElementoTablaSimbolo(const string, const char&) const;
         void evaluarExpresion(string, string); 
         vector<string> generarVectorExpresion(const string&);

         // metodos segunda pasada
         void llenarMemoria();
         void llenarMemoriaConstantes();
         void llenarMemoriaVariables();
         void llenarMemoriaInstrucciones();
         void actualizarUbicacionTS(string, const char&);
         void analizarInstruccion(string&);
         void agregarConstanteMemoria(int&, EntradaTabla&);
         void agregarVariableMemoria(EntradaTabla&);
         void imprimirMemoria();
         int buscarUbicacionElementoTS(const string&, const char&);
         void generarInstruccionInput(string);
         void generarInstruccionPrint(string);
         void generarInstruccionEnd();
         void generarInstruccionGoto(string);
         void vaciarMemoria();
         void vaciarTablaSimbolo();
};

#endif //FRONTEND_HPP