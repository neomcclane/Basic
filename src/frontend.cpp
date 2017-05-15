#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <regex>
#include <cmath>

#include "../headers/frontend.hpp"
#include "../headers/libreria.hpp"

using namespace std;

const int TAM_LINEA = 128;
const char VARIABLE = 'V';
const char CONSTANTE = 'C';
const char LINEA = 'L';

Frontend::Frontend(string nFichero) {
    this->nFichero = nFichero;
    // inicializacion de las expresiones regulares
    eRem = "^([0-9]+)\\s(rem|REM)\\s(.*)$";
    eInput = "^([0-9]+)\\s(input|INPUT)\\s([a-z]|[A-Z])$";
    ePrint = "^([0-9]+)\\s(print|PRINT)\\s([a-z]|[A-Z])$";
    eEnd = "^([0-9]+)\\s(end|END)$";
    eLineaBlanco = "^(\\n*)$";
    
    eIfGoto = "^([0-9]+)\\s(if|IF)\\s([a-zA-Z0-9*=<>\\s]+)\\s(goto|GOTO)\\s([0-9]+)$";
    
    eLet = "^([0-9]+)\\s?(let)\\s?([a-zA-Z])\\s?(=)\\s?([0-9a-zA-Z\\+-/\\*\\s\\(\\)]+|[])$";
    eGoto = "^([0-9]+)\\s(goto|GOTO)\\s([0-9]+)$";


    // inicializacion de los elementos para generar codigo maquina basic
    tablaSimbolo = nullptr;
    memoria = nullptr;
    contadorInstrucciones = 0;
    multiplicador = 100;
}

void Frontend::vaciarMemoria() {
    EstructuraMemoria* m = nullptr;

    while(memoria != nullptr) {
        m = memoria;
        memoria = memoria->sig;
        m->sig = nullptr;
        delete m;
    }
    delete memoria;
    // cout << "\n\nvaciado mamoria" << endl;
}

void Frontend::vaciarTablaSimbolo() {
    EntradaTabla* entrada = nullptr;

    while(tablaSimbolo != nullptr) {
        entrada = tablaSimbolo;
        tablaSimbolo = tablaSimbolo->sig;
        entrada->sig = nullptr;
        delete entrada;
    }

    delete tablaSimbolo;
   
    // cout << "vaciado tablaSimbolo" << endl;
}

Frontend::~Frontend() {
    vaciarMemoria();
    vaciarTablaSimbolo();
}

void Frontend::abrirFichero() {
    ifstream fichero;
    fichero.open(nFichero);
    
    char linea[TAM_LINEA]; 

    while(!fichero.eof()) {
       fichero.getline(linea, TAM_LINEA);
       vLinea.push_back(linea);
    }
    fichero.close();
}

void Frontend::primeraPasada() {
    abrirFichero();
    analizaEstructuraLineas();
}

void Frontend::analizaEstructuraLineas() {
    for(string& linea:vLinea) {
       analizadorLexico(linea);
    }
}

void Frontend::analizadorLexico(string& linea) {
    smatch sm;
    if(regex_match(linea, sm, regex {eRem})) { // es un REM
       agregarElementoTablaSimbolo(sm[1], LINEA);
    } 
    else if(regex_match(linea, sm, regex {eInput})) { // es un INPUT
        agregarElementoTablaSimbolo(sm[1], LINEA);
        agregarElementoTablaSimbolo(sm[3], VARIABLE);
        
    }
    else if(regex_match(linea, sm, regex {eLet})) { // es un LET
        agregarElementoTablaSimbolo(sm[1], LINEA);
        agregarElementoTablaSimbolo(sm[3], VARIABLE);
        string ex = sm[5];
        // string ex = ;
        evaluarExpresion(regex_replace(ex, regex{"\\s"}, ""), sm[1]);
    }
    else if(regex_match(linea, sm, regex {ePrint})) { // es un PRINT
        agregarElementoTablaSimbolo(sm[1], LINEA);
    }
    else if(regex_match(linea, sm, regex {eIfGoto})) { // es un IF GOTO
       agregarElementoTablaSimbolo(sm[1], LINEA);
       string ex = sm[3];
       evaluarExpresion(regex_replace(ex, regex{"\\s"}, ""), sm[1]);

    }
    else if(regex_match(linea, sm, regex {eGoto})) { // es un GOTO
        agregarElementoTablaSimbolo(sm[1], LINEA);
    }
    else if(regex_match(linea, sm, regex {eLineaBlanco})) { // es una LINEA
        //cout << endl;
    }
    else if(regex_match(linea, sm, regex {eEnd})) { // es un END
        agregarElementoTablaSimbolo(sm[1], LINEA);
    }
    else {
        string err = "Error: en '"+linea+"'";
        throw lib::ELexico(err);
    }
}

vector<string> Frontend::generarVectorExpresion(const string& expresion) {
    vector<string> vExpresion;
    string tokens = "";
    string sToken = "";
    //cout << "expresion: " << expresion << endl;
    for(const char cToken:expresion) {
        sToken = cToken;

        if(regex_match(sToken, regex{"([a-zA-Z])"})) {
            if(tokens != "") {
                vExpresion.push_back(tokens);
                tokens = "";
            } 
            vExpresion.push_back(sToken);
        
        }
        else if(regex_match(sToken, regex{"([0-9])"})) {
            tokens += sToken;
        }
        else if(regex_match(sToken, regex{"([\\+\\*-/\\(\\)])"})) {
            if(tokens != "") {
                vExpresion.push_back(tokens);
                tokens = "";
            }
            vExpresion.push_back(sToken);
        }
    }
    if(tokens != "") {
        vExpresion.push_back(tokens);
    }
    return vExpresion;
}

void Frontend::evaluarExpresion(string expresion, string linea) {
    vector<string> vExpresion = generarVectorExpresion(expresion);

    for(string& token:vExpresion) {
        if(regex_match(token, regex{"([a-zA-Z])"}) && !existeElementoTablaSimbolo(token, VARIABLE)) {
            
            throw lib::EVariableNoDeclarada("Error en linea '"+linea+"', '"+token+"' no ha sido declarada.");
        
        }
        else if(regex_match(token, regex{"([0-9]+)"}) && !existeElementoTablaSimbolo(token, CONSTANTE)) {
            agregarElementoTablaSimbolo(token, CONSTANTE);
        }
    }
}

void Frontend::agregarElementoTablaSimbolo(const string& sSimbolo, const char tipo) {
    if(tablaSimbolo == nullptr) {
        if(tipo == LINEA && !existeElementoTablaSimbolo(sSimbolo, tipo)) {
            tablaSimbolo = new EntradaTabla{stoi(sSimbolo), "", tipo, 0};
        }
        else if(tipo == VARIABLE && !existeElementoTablaSimbolo(sSimbolo, tipo)) {
            tablaSimbolo = new EntradaTabla{-1, sSimbolo, tipo, 0};
        }
        else if(tipo == CONSTANTE && !existeElementoTablaSimbolo(sSimbolo, tipo)) {
            tablaSimbolo = new EntradaTabla{stoi(sSimbolo), "", tipo, 0};
        }
    }
    else {
        EntradaTabla* entrada0 = tablaSimbolo;

        while(entrada0->sig != nullptr) {
            entrada0 = entrada0->sig;
        }
        if(tipo == LINEA && !existeElementoTablaSimbolo(sSimbolo, tipo)) {
            entrada0->sig = new EntradaTabla{stoi(sSimbolo), "", tipo, 0};
        }
        else if(tipo == LINEA && existeElementoTablaSimbolo(sSimbolo, tipo)) {
            throw lib::ELineaRepetida("Error, la linea '"+sSimbolo+"' ya se encuentra registrada.");
        }
        else if(tipo == VARIABLE && !existeElementoTablaSimbolo(sSimbolo, tipo)) {
            entrada0->sig = new EntradaTabla{-1, sSimbolo, tipo, 0};
        }
        else if(tipo == CONSTANTE && !existeElementoTablaSimbolo(sSimbolo, tipo)) {
            entrada0->sig = new EntradaTabla{stoi(sSimbolo), "", tipo, 0};
        }

    }
}

void Frontend::imprimirTablaSimbolo() {
    EntradaTabla* entrada = tablaSimbolo;
    cout << "\n\n=====================TABLA SIMBOLO=====================" << endl;
    cout << "\tSimbolo\t\t Tipo \t\t Ubicacion" << endl;

    while(entrada != nullptr) {
        if(entrada->tipo != VARIABLE) {
            cout << "\t  " << entrada->iSimbolo << "\t \t  " << entrada->tipo << "\t \t  " << entrada->ubicacion << endl;
        }
        else {
            cout << "\t  " << entrada->sSimbolo << "\t \t  " << entrada->tipo << "\t \t  " << entrada->ubicacion << endl;
        }
        entrada = entrada->sig;
    }
    
    cout << "\n\n=======================================================" << endl;
}

bool Frontend::existeElementoTablaSimbolo(const string simbolo, const char& tipo) const {
    bool resultado = false;
    EntradaTabla* entrada = tablaSimbolo;

    while(entrada != nullptr) {

        if(tipo == LINEA && tipo == entrada->tipo && entrada->iSimbolo == stoi(simbolo)) {
            resultado = true;
            break;
        } 
        else if(tipo != LINEA && tipo == entrada->tipo && entrada->sSimbolo == simbolo){
            resultado = true;
            break;
        }

        entrada = entrada->sig;
    }

    return resultado;
}

void Frontend::segundaPasada() {
    llenarMemoria();
    imprimirTablaSimbolo();
    imprimirMemoria();
}

void Frontend::llenarMemoria() {
    llenarMemoriaConstantes();
    llenarMemoriaVariables();
}

void Frontend::llenarMemoriaConstantes() {
    EntradaTabla* entrada = tablaSimbolo;

    while(entrada != nullptr) {
        
        if(entrada->tipo == CONSTANTE) {
            agregarConstanteMemoria(entrada->iSimbolo, *entrada);
        }
        
        entrada = entrada->sig;
    }
}

void Frontend::llenarMemoriaVariables() {
    EntradaTabla* entrada = tablaSimbolo;

    while(entrada != nullptr) {
        
        if(entrada->tipo == VARIABLE) {
            agregarVariableMemoria(*entrada);
        }
        
        entrada = entrada->sig;
    }
}

void Frontend::agregarConstanteMemoria(int& constante, EntradaTabla& refEntrada) {
    if(memoria == nullptr) {
        memoria = new EstructuraMemoria{contadorInstrucciones, (constante < 0)?((90*multiplicador)+abs(constante))*(-1):(90*multiplicador)+constante, nullptr};
        refEntrada.ubicacion = contadorInstrucciones;
        contadorInstrucciones++;
    }
    else {
        EstructuraMemoria* auxMemoria = memoria;

        while(auxMemoria->sig != nullptr) {
            auxMemoria = auxMemoria->sig;
        } 

        auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones, (constante < 0)?((90*multiplicador)+abs(constante))*(-1):(90*multiplicador)+constante, nullptr};
        refEntrada.ubicacion = contadorInstrucciones;
        contadorInstrucciones++;
    }
    
}

void Frontend::agregarVariableMemoria(EntradaTabla& refEntrada) {

    if(memoria == nullptr) {
        memoria = new EstructuraMemoria{contadorInstrucciones, (91*multiplicador), nullptr};
        refEntrada.ubicacion = contadorInstrucciones;
        contadorInstrucciones++;
    }
    else {
        EstructuraMemoria* auxMemoria = memoria;

        while(auxMemoria->sig != nullptr) {
            auxMemoria = auxMemoria->sig;
        } 

        auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones, (91*multiplicador), nullptr};
        refEntrada.ubicacion = contadorInstrucciones;
        contadorInstrucciones++;
    }
}

void Frontend::imprimirMemoria() {
    EstructuraMemoria* auxMemoria = memoria;
    cout << "\n\n=====================TABLA MEMORIA=====================" << endl;
    cout << "\tInstruccion\t\t Ubicacion" << endl;

    while(auxMemoria != nullptr) {
        cout << "\t  " << auxMemoria->instruccion << "\t \t\t  " << auxMemoria->ubicacion << endl;
        auxMemoria = auxMemoria->sig;
    }
    
    cout << "\n\n=======================================================" << endl;
}

void Frontend::analizadorSintactico() {

}