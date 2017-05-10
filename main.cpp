#include <iostream>
#include <cstdlib>

#include "headers/compilador.hpp"
#include "headers/libreria.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    string nFichero;
    
    try {
        nFichero = argv[1];
        lib::validarFichero(nFichero);

        Compilador* app = new Compilador(nFichero);
        app->run();

        delete app;

    }
    catch(const lib::ENombreFichero& e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
    catch(const lib::EFicheroNoExiste& e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
    catch(const lib::ELexico& e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
    catch(...) {
        cerr << "Error generico" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}