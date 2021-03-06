#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <regex>
#include <cmath>

#include "../headers/frontend.hpp"
#include "../headers/libreria.hpp"

using namespace std;
using namespace lib;

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
    contadorInstruccionesAux = 0;
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

EstructuraMemoria* Frontend::segundaPasada() {
    llenarMemoria();
    resolverReferenciasAdelantadas();
    // imprimirTablaSimbolo();
    // imprimirMemoria();
    return memoria;
}

void Frontend::resolverReferenciasAdelantadas() {
    EstructuraMemoria* memo = memoria;

    while(memo != nullptr) {
        if(regex_match(to_string(memo->instruccion), regex {"^(40[0-9]{2,})$"})) {
            string operando = to_string(memo->instruccion%100);
            memo->instruccion = (40*100)+buscarUbicacionElementoTS(operando, LINEA);
        }
        else if(regex_match(to_string(memo->instruccion), regex {"^(41[0-9]{2,})$"})) {
            string operando = to_string(memo->instruccion%100);
            memo->instruccion = (41*100)+buscarUbicacionElementoTS(operando, LINEA);
        }
        else if(regex_match(to_string(memo->instruccion), regex {"^(42[0-9]{2,})$"})) {
            string operando = to_string(memo->instruccion%100);
            memo->instruccion = (42*100)+buscarUbicacionElementoTS(operando, LINEA);
        }
        memo = memo->sig;
    }
}

void Frontend::llenarMemoria() {
    llenarMemoriaConstantes();
    llenarMemoriaVariables();
    llenarMemoriaInstrucciones();
    
}

void Frontend::llenarMemoriaInstrucciones() {
    for(string& linea:vLinea) {
        analizarInstruccion(linea);
    }
}

void Frontend::analizarInstruccion(string& instruccion) {
    smatch sm;
    if(regex_match(instruccion, sm, regex {eRem})) {
        actualizarUbicacionTS(sm[1], LINEA);
    }
    else if(regex_match(instruccion, sm, regex {eInput})) {
        actualizarUbicacionTS(sm[1], LINEA);   
        generarInstruccionInput(sm[3]);       
    }
    else if(regex_match(instruccion, sm, regex {eLet})) {
        actualizarUbicacionTS(sm[1], LINEA);
        generarInstruccionLet(sm[3], sm[5]);
    }
    else if(regex_match(instruccion, sm, regex {ePrint})) {
        actualizarUbicacionTS(sm[1], LINEA);
        generarInstruccionPrint(sm[3]);
    }   
    else if(regex_match(instruccion, sm, regex {eIfGoto})) {
        actualizarUbicacionTS(sm[1], LINEA);
        generarInstruccionIfGoto(sm[3], sm[5]);
    }
    else if(regex_match(instruccion, sm, regex {eGoto})) {
        actualizarUbicacionTS(sm[1], LINEA);
        generarInstruccionGoto(sm[3]);        
    }
    else if(regex_match(instruccion, sm, regex {eEnd})) {
        actualizarUbicacionTS(sm[1], LINEA);
        generarInstruccionEnd();
    }
}

void Frontend::generarInstruccionIfGoto(string comparacion, string linea) {
    comparacion = regex_replace(comparacion, regex {"\\s"},"");
    analizarComparacionIfGoto(comparacion, linea);
}

void Frontend::analizarComparacionIfGoto(string& comparacion, string& salto) {
    string auxString = "";
    vector<string> arrayComparacion;
    for(char& token:comparacion) {
        auxString = token;
        arrayComparacion.push_back(auxString);
    }
    
    EstructuraMemoria* auxMemoria = memoria;
    while(auxMemoria->sig != nullptr) {auxMemoria=auxMemoria->sig;}

    string operador = "";
    if(arrayComparacion.size() == 3) {
        operador = arrayComparacion[1];
        if(operador == "<") {
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (20*multiplicador)+buscarUbicacionElementoTS(arrayComparacion[0]), nullptr};
            auxMemoria = auxMemoria->sig;
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (31*multiplicador)+buscarUbicacionElementoTS(arrayComparacion[2]), nullptr};
            auxMemoria = auxMemoria->sig;
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (41*multiplicador)+stoi(salto), nullptr};
        }
        else if(operador == ">") {
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (20*multiplicador)+buscarUbicacionElementoTS(arrayComparacion[2]), nullptr};            
            auxMemoria = auxMemoria->sig;            
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (31*multiplicador)+buscarUbicacionElementoTS(arrayComparacion[0]), nullptr};
            auxMemoria = auxMemoria->sig;                        
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (41*multiplicador)+stoi(salto), nullptr};
        }        
    }
    else if(arrayComparacion.size() == 4) {
        operador = arrayComparacion[1]+""+arrayComparacion[2];        
        if(operador == "==") {
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (20*multiplicador)+buscarUbicacionElementoTS(arrayComparacion[0]), nullptr};
            auxMemoria = auxMemoria->sig;            
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (31*multiplicador)+buscarUbicacionElementoTS(arrayComparacion[3]), nullptr};
            auxMemoria = auxMemoria->sig;            
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (42*multiplicador)+stoi(salto), nullptr};
        }
        else if(operador == "<=") {
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (20*multiplicador)+buscarUbicacionElementoTS(arrayComparacion[0]), nullptr};
            auxMemoria = auxMemoria->sig;            
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (31*multiplicador)+buscarUbicacionElementoTS(arrayComparacion[3]), nullptr};
            auxMemoria = auxMemoria->sig;            
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (42*multiplicador)+stoi(salto), nullptr};
            auxMemoria = auxMemoria->sig;            
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (41*multiplicador)+stoi(salto), nullptr};
        }
        else if(operador == ">=") {
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (20*multiplicador)+buscarUbicacionElementoTS(arrayComparacion[3]), nullptr};
            auxMemoria = auxMemoria->sig;            
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (31*multiplicador)+buscarUbicacionElementoTS(arrayComparacion[0]), nullptr};
            auxMemoria = auxMemoria->sig;            
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (42*multiplicador)+stoi(salto), nullptr};
            auxMemoria = auxMemoria->sig;            
            auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, (41*multiplicador)+stoi(salto), nullptr};
        }
    }    
}

void Frontend::generarInstruccionLet(string variable, string operacion) {
    operacion = regex_replace(operacion, regex{"\\s"}, "");
    Pila* postfijo = infijoAPostfijo(operacion);
    
    generarInstruccionLet(postfijo, variable);
}


void Frontend::agregarElementoMemoria(EstructuraMemoria** memo, int& instruccion) {
    if((*memo) == nullptr) {
        (*memo) = new EstructuraMemoria{contadorInstrucciones++, instruccion, nullptr};
    }
    else {
        EstructuraMemoria* ptrMemoria = (*memo);
        while(ptrMemoria->sig != nullptr) {ptrMemoria=ptrMemoria->sig;}

        ptrMemoria->sig = new EstructuraMemoria{contadorInstrucciones++, instruccion, nullptr};
    }
}

void Frontend::agregarElementoMemoria(EstructuraMemoria** auxMemoria, string& simbolo) {
    if((*auxMemoria) == nullptr) {
        (*auxMemoria) = new EstructuraMemoria{contadorInstrucciones++, (20*multiplicador)+buscarUbicacionElementoTS(simbolo), nullptr};
    }
    else {
        EstructuraMemoria* memo = (*auxMemoria);
        while(memo->sig != nullptr) {memo = memo->sig;}

        memo->sig = new EstructuraMemoria{contadorInstrucciones++, (20*multiplicador)+buscarUbicacionElementoTS(simbolo), nullptr}; 
    }
}

void Frontend::agregarElementoOperacionMemoria(EstructuraMemoria** auxMemoria, Pila** auxPostfijo) {
    // imprimirPostfijo((*auxPostfijo));
    string  variable = (*auxPostfijo)->simbolo;
    Pila* pPila = (*auxPostfijo);
    (*auxPostfijo) = (*auxPostfijo)->sig;
    (*auxPostfijo)->ant = nullptr;
    delete pPila;
    string tipo;
    Pila* auxPila = (*auxPostfijo);
    
    int c = 0;
    while(auxPila != nullptr) {
        if(regex_match(auxPila->simbolo, regex {"^[\\+-/\\*]$"})) {
            tipo = auxPila->simbolo;
            break;
        }
        c++;
        Pila* pAnt = auxPila; 
        auxPila = auxPila->sig;
        auxPila->ant = pAnt;
    }
    
    if(auxPila->ant != nullptr) {
        Pila* auxPila0 = auxPila->ant;
        auxPila0->sig = auxPila->sig;
    } 
    else {
        (*auxPostfijo) = (*auxPostfijo)->sig;
    }
    delete auxPila;
    
    EstructuraMemoria* memo = (*auxMemoria);
    while(memo->sig != nullptr) {memo=memo->sig;}
    

    if(tipo == "+") {
       memo->sig = new EstructuraMemoria{contadorInstrucciones++, (30*multiplicador)+abs(buscarUbicacionElementoTS(variable)), nullptr};
    }
    else if(tipo == "-") {
        memo->sig = new EstructuraMemoria{contadorInstrucciones++, (31*multiplicador)+abs(buscarUbicacionElementoTS(variable)), nullptr};
    }
    else if(tipo == "/") {
        memo->sig = new EstructuraMemoria{contadorInstrucciones++, (32*multiplicador)+abs(buscarUbicacionElementoTS(variable)), nullptr};
    }
    else if(tipo == "*") {
        memo->sig = new EstructuraMemoria{contadorInstrucciones++, (33*multiplicador)+abs(buscarUbicacionElementoTS(variable)), nullptr};
    }

    agregarVariableTS(auxMemoria, auxPostfijo);
}

void Frontend::agregarVariableTS(EstructuraMemoria** auxMemoria, Pila** auxPostfijo) {
    EntradaTabla* entrada = tablaSimbolo;
    while(entrada->sig != nullptr) {entrada = entrada->sig;}
    string instruccion = "_"+to_string(contadorInstruccionesAux++);
    entrada->sig = new EntradaTabla{-1, instruccion, VARIABLE, contadorInstrucciones, nullptr};

    EstructuraMemoria* memo = (*auxMemoria);
    while(memo->sig != nullptr) {memo = memo->sig;}   
    int auxUbicacion = contadorInstrucciones;
    memo->sig = new EstructuraMemoria{contadorInstrucciones++, (91*multiplicador), nullptr};
    memo->sig->sig = new EstructuraMemoria{contadorInstrucciones++, (21*multiplicador)+(auxUbicacion), nullptr};
    
    Pila* pila = new Pila{instruccion, true, (*auxPostfijo), nullptr};
    (*auxPostfijo) = pila;
}

void Frontend::eliminarElementoPila(Pila** pila, string& simbolo) {
    Pila* auxPila = (*pila);

    while(auxPila != nullptr) {
        if(auxPila->simbolo == simbolo) {
            break;
        } 
        auxPila = auxPila->sig;
    }

    if(auxPila != nullptr) {
        if(auxPila->ant != nullptr) {
            Pila* auxPila1 = auxPila->ant;
            auxPila1->sig = auxPila->sig;
        }
        else {
            (*pila) = (*pila)->sig;
            (*pila)->ant = nullptr;
        }
        delete auxPila;
    }
}

void Frontend::generarInstruccionMemoria(Pila** auxPostfijo, EstructuraMemoria** auxMemoria, string& variable) {
    
    if((*auxPostfijo) != nullptr && (*auxPostfijo)->tamano() > 1) {
        agregarElementoMemoria(&(*auxMemoria), (*auxPostfijo)->simbolo);
        eliminarElementoPila(&(*auxPostfijo), (*auxPostfijo)->simbolo);
        
        agregarElementoOperacionMemoria(&(*auxMemoria), &(*auxPostfijo));
        generarInstruccionMemoria(auxPostfijo, auxMemoria, variable);
    }  
    else if((*auxPostfijo) != nullptr && (*auxPostfijo)->tamano() == 1) {
        if((*auxMemoria) == nullptr) {
            (*auxMemoria) = new EstructuraMemoria {contadorInstrucciones++, (20*multiplicador)+abs(buscarUbicacionElementoTS((*auxPostfijo)->simbolo)), nullptr};
            (*auxMemoria)->sig = new EstructuraMemoria {contadorInstrucciones++, (21*multiplicador)+abs(buscarUbicacionElementoTS(variable)), nullptr};
            
        }   
        else {
            EstructuraMemoria* aux = (*auxMemoria);
            while(aux->sig != nullptr) {aux = aux->sig;}
            aux->sig = new EstructuraMemoria {contadorInstrucciones++, (20*multiplicador)+buscarUbicacionElementoTS((*auxPostfijo)->simbolo), nullptr};
            aux->sig->sig = new EstructuraMemoria {contadorInstrucciones++, (21*multiplicador)+buscarUbicacionElementoTS(variable), nullptr};
        }
    }

}

int Frontend::buscarUbicacionElementoTS(const string& variable) {
    char tipo = VARIABLE;
    int ubicacion = -1;
    if(regex_match(variable, regex {"^[0-9]+$"})) {
        tipo = CONSTANTE;
    }

    EntradaTabla* entrada = tablaSimbolo;
    while(entrada != nullptr) {
        if(tipo == CONSTANTE && tipo == entrada->tipo && entrada->iSimbolo == stoi(variable)) {
            ubicacion = entrada->ubicacion;
            break;
        }
        else if(tipo == VARIABLE &&  tipo == entrada->tipo && entrada->sSimbolo == variable) {
            ubicacion = entrada->ubicacion;
            break;
        }
        entrada = entrada->sig;
    }
    return ubicacion;
}

void Frontend::vaciarMemoriaAux(EstructuraMemoria* auxMemoria) {
    EstructuraMemoria* memo = memoria;
    while(memo->sig != nullptr) {memo = memo->sig;}

    EstructuraMemoria* aux = auxMemoria;
    while(aux != nullptr) {
        memo->sig = new EstructuraMemoria {aux->ubicacion, aux->instruccion, nullptr};
        memo = memo->sig;
        aux = aux->sig;
    }
}

void Frontend::generarInstruccionLet(Pila* postfijo, string& variable) {
    EstructuraMemoria* ptrMemoria = nullptr;
    generarInstruccionMemoria(&postfijo, &ptrMemoria, variable);
    vaciarMemoriaAux(ptrMemoria);
    Pila* auxPostfijo = postfijo;
    while(postfijo != nullptr) {
        auxPostfijo = postfijo;
        postfijo = postfijo->sig;
        delete auxPostfijo;
    }
    auxPostfijo = nullptr;
    
    EstructuraMemoria* auxMemoria = nullptr;
    while(ptrMemoria != nullptr) {
        auxMemoria = ptrMemoria;
        ptrMemoria = ptrMemoria->sig;
        delete auxMemoria;
    }
    auxMemoria = nullptr;
}

Pila* Frontend::infijoAPostfijo(string& operacion) {
    Pila* postfijo = nullptr;
    string infijo = operacion + ")";
    string auxToken = ""; // ojo, acomodar este metodo de asignacion de char to string
    string contToken = "";

    Pila* pila = new Pila{"(", true, nullptr, nullptr};

    for(char& token:infijo) {
        auxToken += token;
        //----------------------
        if(regex_match(auxToken, regex {"^[0-9a-zA-Z]$"})) {
            contToken += auxToken;
        }
        else {
            if(contToken.length() != 0) {                
                agregarElementoPostfijo(&postfijo, contToken);
                contToken = "";
            }
            if(regex_match(auxToken, regex{"^[\\+-/\\*]$"})) {
                evaluarOperadorPostfijo(&pila, &postfijo, auxToken);
            }
            else if(regex_match(auxToken, regex{"^[\\(]$"})) {
                agregarElementoPila(&pila, auxToken);
            }
            else if(regex_match(auxToken, regex{"^[\\)]$"})) {
                evaluarOperadorPostfijo(&pila, &postfijo, auxToken);
            }
            contToken += auxToken;
            contToken = "";
        }
        
        auxToken = "";
    }
    while(pila != nullptr) {
        Pila* auxPila = pila;
        pila = pila->sig; 
        delete auxPila;
    } 
    return postfijo;
}

void Frontend::evaluarOperadorPostfijo(Pila** ptrPila, Pila** ptrPostfijo, string& operador) {
    Pila* auxPila = *ptrPila;
    int tipo = -1;
    
    while(auxPila->sig != nullptr) {auxPila = auxPila->sig;}
    if(regex_match(operador, regex {"^\\+-$"})) { tipo = 0;} 
    else if(regex_match(operador, regex {"^[\\*/]$"})) {tipo = 1;}
    else if(regex_match(operador, regex {"^[\\)]$"})) {tipo = 2;}

    while(auxPila != nullptr) {
        if(auxPila->estado && regex_match(operador, regex {"^[\\(]$"})) {
            if(tipo==2){auxPila->estado=false;}
            break;
        }   
        else if((tipo == 0 || tipo == 2) && auxPila->estado && regex_match(auxPila->simbolo, regex {"^[\\+-/\\*]$"})){
            agregarElementoPostfijo(&(*ptrPostfijo), auxPila->simbolo);
            auxPila->estado = false;
        }
        else if(tipo == 1 && auxPila->estado && regex_match(auxPila->simbolo, regex {"^[\\*/]$"})){
            agregarElementoPostfijo(&(*ptrPostfijo), auxPila->simbolo);
            auxPila->estado = false;
        }
        auxPila = auxPila->ant;
    }
    
    if(tipo!=2) {agregarElementoPila(&(*ptrPila), operador);} 
}

void Frontend::agregarElementoPila(Pila** ptrPila, string& operador) {
    Pila* auxPila = *ptrPila;
    while(auxPila->sig != nullptr) { auxPila=auxPila->sig; }

    auxPila->sig = new Pila{operador, true, nullptr, auxPila};
}

void Frontend::agregarElementoPostfijo(Pila** ptrPostfijo, string& simbolo) {
    if(*ptrPostfijo == nullptr) {
        *ptrPostfijo = new Pila{simbolo, true, nullptr, nullptr};
    }
    else {
        Pila* auxPostfijo = *ptrPostfijo;
        while(auxPostfijo->sig != nullptr) {auxPostfijo = auxPostfijo->sig;}
        auxPostfijo->sig = new Pila{simbolo, true, nullptr, auxPostfijo};        
    }
}

void Frontend::generarInstruccionGoto(string sSimbolo) {
    EstructuraMemoria* auxMemoria = memoria;

    while(auxMemoria->sig != nullptr) {
        auxMemoria = auxMemoria->sig;
    }
    auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones, (40*multiplicador)+stoi(sSimbolo), nullptr};
    contadorInstrucciones++;
}

void Frontend::generarInstruccionEnd() {
    EstructuraMemoria* auxMemoria = memoria;

    while(auxMemoria->sig != nullptr) {
        auxMemoria = auxMemoria->sig;
    }

    auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones, 43 * multiplicador, nullptr};
}

void Frontend::generarInstruccionPrint(string sSimbolo) {
    EstructuraMemoria* auxMemoria = memoria;

    while(auxMemoria->sig != nullptr) {
        auxMemoria = auxMemoria->sig;
    }

    int ubicacion = buscarUbicacionElementoTS(sSimbolo, VARIABLE);

    if(ubicacion != -1) {
        auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones, 11 * multiplicador + ubicacion, nullptr};
        contadorInstrucciones++;
    }
    else {
        throw lib::EErrorUbicacionTS("Error, en la busqueda de la ubicacion dentro de la TS.");        
    }
}

void Frontend::generarInstruccionInput(string sSimbolo) {
    EstructuraMemoria* auxMemoria = memoria;

    while(auxMemoria->sig != nullptr) {
        auxMemoria = auxMemoria->sig;
    }

    int ubicacion = buscarUbicacionElementoTS(sSimbolo, VARIABLE);
    
    if(ubicacion != -1) {
        auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones, 10*multiplicador+ubicacion, nullptr};
        contadorInstrucciones++;
    }
    else {
        throw lib::EErrorUbicacionTS("Error, en la busqueda de la ubicacion dentro de la TS.");
    }
}

int Frontend::buscarUbicacionElementoTS(const string& sSimbolo, const char& tipo) {
    int ubicacion = -1;
    EntradaTabla* entrada = tablaSimbolo;
    while(entrada != nullptr) {
        if(tipo == VARIABLE && entrada->tipo == tipo && entrada->sSimbolo == sSimbolo) {
            ubicacion = entrada->ubicacion;
            break;
        }
        else if(tipo != VARIABLE && entrada->tipo == tipo && entrada->iSimbolo == stoi(sSimbolo)) {
            ubicacion = entrada->ubicacion;
            break;
        }
        entrada = entrada->sig;
    }
    return ubicacion;
}

void Frontend::actualizarUbicacionTS(string sSimbolo, const char& tipo) {
    if(tipo == LINEA) {
        int iSimbolo = stoi(sSimbolo);
        EntradaTabla* entrada = tablaSimbolo;
        while(entrada != nullptr) {
            if(entrada->iSimbolo == iSimbolo && entrada->tipo == LINEA) {
                entrada->ubicacion = contadorInstrucciones;
                break;
            }

            entrada = entrada->sig;
        }
    }

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
            tablaSimbolo = new EntradaTabla{stoi(sSimbolo), "", tipo, 0, nullptr};
        }
        else if(tipo == VARIABLE && !existeElementoTablaSimbolo(sSimbolo, tipo)) {
            tablaSimbolo = new EntradaTabla{-1, sSimbolo, tipo, 0, nullptr};
        }
        else if(tipo == CONSTANTE && !existeElementoTablaSimbolo(sSimbolo, tipo)) {
            tablaSimbolo = new EntradaTabla{stoi(sSimbolo), "", tipo, 0, nullptr};
        }
    }
    else {
        EntradaTabla* entrada0 = tablaSimbolo;

        while(entrada0->sig != nullptr) {
            entrada0 = entrada0->sig;
        }
        if(tipo == LINEA && !existeElementoTablaSimbolo(sSimbolo, LINEA)) {
            entrada0->sig = new EntradaTabla{stoi(sSimbolo), "", tipo, 0, nullptr};
        }
        else if(tipo == LINEA && existeElementoTablaSimbolo(sSimbolo, LINEA)) {
            throw lib::ELineaRepetida("Error, la linea '"+sSimbolo+"' ya se encuentra registrada.");
        }
        else if(tipo == VARIABLE && !existeElementoTablaSimbolo(sSimbolo, VARIABLE)) {
            entrada0->sig = new EntradaTabla{-1, sSimbolo, tipo, 0, nullptr};
        }
        else if(tipo == CONSTANTE && !existeElementoTablaSimbolo(sSimbolo, CONSTANTE)) {
            entrada0->sig = new EntradaTabla{stoi(sSimbolo), "", tipo, 0, nullptr};
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

        if(tipo==LINEA && tipo == entrada->tipo && entrada->iSimbolo == stoi(simbolo)) {
            resultado = true;
            break;
        } 
        if(tipo==CONSTANTE && tipo == entrada->tipo && entrada->iSimbolo == stoi(simbolo)) {
            resultado = true;
            break;
        } 
        else if(tipo==VARIABLE && tipo == entrada->tipo && entrada->sSimbolo == simbolo){
            resultado = true;
            break;
        }

        entrada = entrada->sig;
    }

    return resultado;
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
    string sConstante = to_string(constante);
    if(memoria == nullptr) {
        memoria = new EstructuraMemoria{contadorInstrucciones, constante, nullptr};
        refEntrada.ubicacion = contadorInstrucciones;
        contadorInstrucciones++;
    }
    else {
        EstructuraMemoria* auxMemoria = memoria;
        while(auxMemoria->sig != nullptr) {auxMemoria = auxMemoria->sig;} 

        auxMemoria->sig = new EstructuraMemoria{contadorInstrucciones, constante, nullptr};
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
        if(auxMemoria->ubicacion != -1) {
            cout << "\t  " << auxMemoria->instruccion << "\t \t\t  " << auxMemoria->ubicacion << endl;
        }
        else {
            cout << "\t  " << auxMemoria->instruccion << endl;
        }
        auxMemoria = auxMemoria->sig;
    }
    
    cout << "\n\n=======================================================" << endl;
}

void Frontend::analizadorSintactico() {

}