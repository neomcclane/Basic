#include <iostream>
#include <cstdlib>

#include "headers/compilador.hpp"
#include "headers/libreria.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    string nFichero;
    Compilador* app = nullptr;

    try {
        nFichero = argv[1];
        lib::validarFichero(nFichero);

        app = new Compilador(nFichero);
        app->run();

        

    }
    catch(const lib::ENombreFichero& e) {
        cerr << e.what() << endl;
        delete app;
        return EXIT_FAILURE;
    }
    catch(const lib::EFicheroNoExiste& e) {
        cerr << e.what() << endl;
        delete app;
        return EXIT_FAILURE;
    }
    catch(const lib::ELexico& e) {
        cerr << e.what() << endl;
        delete app;
        return EXIT_FAILURE;
    }
    catch(const lib::EVariableNoDeclarada& e) {
        cerr << e.what() << endl;
        delete app;
        return EXIT_FAILURE;
    }
    catch(const lib::ELineaRepetida& e) {
        cerr << e.what() << endl;
        delete app;
        return EXIT_FAILURE;
    }
    catch(const lib::EErrorUbicacionTS& e) {
        cerr << e.what() << endl;
        delete app;
        return EXIT_FAILURE;
    }
    catch(...) {
        cerr << "Error generico" << endl;
        delete app;
        return EXIT_FAILURE;
    }
    delete app;
    return EXIT_SUCCESS;
}