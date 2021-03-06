#ifndef FRONTEND_HPP
#define FRONTEND_HPP

#include <iostream>
#include <vector>
#include <regex>
#include "libreria.hpp"

using namespace std;
using namespace lib;

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
        int contadorInstruccionesAux;

    protected: // metodos
         void primeraPasada();
         EstructuraMemoria* segundaPasada();
    
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
         void generarInstruccionLet(string, string);
         void generarInstruccionIfGoto(string, string);
         Pila* infijoAPostfijo(string&);
         void agregarElementoPostfijo(Pila** ,string&);
         void evaluarOperadorPostfijo(Pila**, Pila**, string&);
         void agregarElementoPila(Pila**, string&);
         void generarInstruccionLet(Pila*, string&);
         void generarInstruccionMemoria(Pila**, EstructuraMemoria**, string&);
         void vaciarMemoriaAux(EstructuraMemoria*);
         int buscarUbicacionElementoTS(const string&);
         void agregarElementoMemoria(EstructuraMemoria**, string&);
         void agregarElementoMemoria(EstructuraMemoria**, int&);
         void eliminarElementoPila(Pila**, string&);
         void agregarElementoOperacionMemoria(EstructuraMemoria**, Pila**);
         void agregarVariableTS(EstructuraMemoria**, Pila**);
         void vaciarMemoria();
         void vaciarTablaSimbolo();

         void resolverReferenciasAdelantadas();

         void analizarComparacionIfGoto(string&, string&);
};

#endif //FRONTEND_HPP