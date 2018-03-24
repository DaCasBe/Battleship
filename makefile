OBJETOS_S = servidor.o funcionesServidor.o usuario.o barco.o tablero.o partida.o
OBJETOS_C = cliente.o funcionesCliente.o
CPPFLAGS = -c -g -Wall -ansi

objetivo: cliente-servidor
	
#Opciones para NO depurar
ndebug: CPPFLAGS += -DNDEBUG 
ndebug: objetivo

cliente-servidor: servidor.exe cliente.exe

servidor.exe: servidor.cpp $(OBJETOS_S) 
	g++ $(OBJETOS_S) -o servidor.exe

cliente.exe: cliente.cpp $(OBJETOS_C)
	g++ $(OBJETOS_C) -o cliente.exe

servidor.o: servidor.cpp
					g++ $(CPPFLAGS) servidor.cpp -o servidor.o

cliente.o: cliente.cpp
					g++ $(CPPFLAGS) cliente.cpp -o cliente.o
					
funcionesServidor.o: funcionesServidor.cpp funcionesServidor.hpp 
					g++ $(CPPFLAGS) funcionesServidor.cpp -o funcionesServidor.o
 
funcionesCliente.o: funcionesCliente.cpp funcionesCliente.hpp 
					g++ $(CPPFLAGS) funcionesCliente.cpp -o funcionesCliente.o

usuario.o: usuario.cpp usuario.hpp
					g++ $(CPPFLAGS) usuario.cpp -o usuario.o

barco.o: barco.cpp barco.hpp 
					g++ $(CPPFLAGS) barco.cpp -o barco.o

tablero.o: tablero.cpp tablero.hpp
					g++ $(CPPFLAGS) tablero.cpp -o tablero.o
					
partida.o: partida.cpp partida.hpp
					g++ $(CPPFLAGS) partida.cpp -o partida.o
					
.PHONY: clean  
clean:
	rm *.o servidor.exe cliente.exe
