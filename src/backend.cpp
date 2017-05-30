#include <iostream>

#include "../headers/backend.hpp"
#include "../headers/libreria.hpp"

using namespace std;
using namespace lib;

Backend::Backend() {
    acumulador = 0;
    contadorInstrucciones = 0;
    registroInstruccion = 0;
    codigoOperacion = 0;
    operando = 0;
}

Backend::~Backend() {
    // cout << "destructor Backend" << endl;
}

void Backend::analizarMemoria(lib::EstructuraMemoria* eMemoria) {
    bool salida = false;
    
    EstructuraMemoria* aux = eMemoria;
    int contador = 0;
    while(aux != nullptr) {
        contador++;
        aux = aux->sig;
    }

    int memoria[contador];
    aux = eMemoria;
    contador = 0;
    while(aux != nullptr) {
        memoria[contador++] = aux->instruccion;
        aux = aux->sig;
    }
    
    while(!salida) {
        registroInstruccion = memoria[contadorInstrucciones];
        codigoOperacion = registroInstruccion/100;
        operando = registroInstruccion%100; 
        
        switch(codigoOperacion) {
            case 10:
                cin >>  memoria[operando];
                contadorInstrucciones++;
                break;

            case 11:
                cout << memoria[operando] << endl;
                contadorInstrucciones++;
                break;

            case 20:
                acumulador = memoria[operando];
                contadorInstrucciones++;                
                break;
            
            case 21:
                memoria[operando] = acumulador;
                contadorInstrucciones++;
                break;
            
            case 30:
                acumulador += memoria[operando];
                contadorInstrucciones++;
                break;
            
            case 31:
                acumulador -= memoria[operando];
                contadorInstrucciones++;
                break;
            
            case 32:
                acumulador /= memoria[operando];
                contadorInstrucciones++;
                break;
            
            case 33:
                acumulador *= memoria[operando];
                contadorInstrucciones++;
                break;
            
            case 40:
                contadorInstrucciones = operando;
                break;
            
            case 41:
                if(acumulador < 0) {contadorInstrucciones = operando;}
                else {contadorInstrucciones++;}
                break;
            
            case 42:
                if(acumulador == 0) {contadorInstrucciones = operando;}
                else {contadorInstrucciones++;}
                break;

            case 43:
                salida = true;
                break;

            default:
                contadorInstrucciones++;
        }
    }
}