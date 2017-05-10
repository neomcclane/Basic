all: basic

basic: main.o compilador.o frontend.o libreria.o
	g++ -o basic main.o compilador.o frontend.o libreria.o -std=gnu++11
	cls
	basic fichero.bsl

main.o:main.cpp headers/compilador.hpp headers/libreria.hpp
	g++ -c main.cpp -std=gnu++11

compilador.o: src/compilador.cpp headers/compilador.hpp headers/frontend.hpp 
	g++ -c src/compilador.cpp -std=gnu++11

frontend.o: src/frontend.cpp headers/frontend.hpp headers/libreria.hpp
	g++ -c src/frontend.cpp -std=gnu++11

libreria.o: src/libreria.cpp headers/libreria.hpp
	g++ -c src/libreria.cpp -std=gnu++11

clean:
	del *.exe
	del *.o