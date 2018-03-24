#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <csignal>

#include "funcionesCliente.hpp"
#include <string>
#include <vector>
#include "macros.hpp"

int sd;

void manejador(int signum);

int main(int argc,char ** argv){
	if(argc!=2){ //No se ha introducido ninguna direccion IP
		std::cout << "Error de sintaxis: ./cliente.exe <direccionIP>" << std::endl;
		
		exit(EXIT_FAILURE);
	}
	
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/

	struct sockaddr_in sockname;
	char buffer[250];
	socklen_t len_sockname;
	fd_set readfds;
	fd_set auxfds;
	int fin=0;
	
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd=socket(AF_INET,SOCK_STREAM,0);
  	
	if(sd==-1){ //No se ha podido abrir el socket
		perror("No se puede abrir el socket cliente\n");
		
    		exit(1);	
	}

	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family=AF_INET;
	sockname.sin_port=htons(2050);
	sockname.sin_addr.s_addr= inet_addr(argv[1]);

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname=sizeof(sockname);
	
	if(connect(sd,(struct sockaddr *)&sockname,len_sockname)==-1){
		perror("Error de conexión");
		
		exit(1);
	}
    
	//Inicializamos las estructuras
	FD_ZERO(&auxfds);
	FD_ZERO(&readfds);
	FD_SET(0,&readfds);
	FD_SET(sd,&readfds);
	

    //Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT,manejador);

	//Mostramos las instrucciones de uso
	std::cout << IRED << "Instrucciones de uso: " << std::endl;
	std::cout << "    REGISTRO -u <nombre> -p <password>: registrarse en el juego" << std::endl;
	std::cout << "    USUARIO <nombre>: introducir un nombre de usuario" << std::endl;
	std::cout << "    PASSWORD <password>: introducir una password para loguearse" << std::endl;
	std::cout << "    INICIAR-PARTIDA: entrar en cola para esperar a poder jugar" << std::endl;
	std::cout << "    DISPARO <letra>,<numero>: disparar al oponente en una casilla en concreto" << RESET << std::endl << std::endl << std::endl << std::endl;
    
	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do{
        	auxfds=readfds;
        	select(sd+1,&auxfds,NULL,NULL,NULL);
        
		if(FD_ISSET(sd,&auxfds)){ //Tengo mensaje desde el servidor
			bzero(buffer,sizeof(buffer));
			recv(sd,buffer,sizeof(buffer),0); //Se recibe un mensaje del servidor
            		std::vector <std::string> splitted_buffer = dividirCadena(std::string(buffer), " "); //Se analiza el mensaje
			
			if (splitted_buffer[0]=="-Err." or splitted_buffer[0]=="+Ok."){ //El mensaje es un mensaje de error o de confirmacion
			std::cout  <<buffer << std::endl;
			if(strcmp(buffer,"-Err. Demasiados clientes conectados\n")==0){ //Hay demasiados clientes conectados con el servidor
				fin=1;
			}
            
			if(strcmp(buffer,"+Ok. Desconexion del cliente")==0){ //Se desconecta del servidor
				fin=1;

			}

			if(strcmp(buffer,"+Ok. Desconexion del servidor")==0){ //Se desconecta del servidor
				fin=1;

			}

			}else{ //El mensaje es un tablero de una partida
				char matriz[10][10];
				deVectoraMatriz(matriz, buffer);
				imprimeMatriz(matriz);
			}
		}
		
		else{ //He introducido información por teclado
			if(FD_ISSET(0,&auxfds)){
				//Se lee el mensaje introducido
				bzero(buffer,sizeof(buffer));
				fgets(buffer,sizeof(buffer),stdin);
                
				/*if(strcmp(buffer,"SALIR\n")==0){ //El mensaje es "SALIR"
					fin=1;
				}*/
                
				send(sd,buffer,sizeof(buffer),0); //Se manda el mensaje al servidor
			}
		}		
	}while(fin==0);
		
	close(sd); //Se cierra el socket

	return 0;
}

void manejador(int signum){


	char buffer[250];

	send(sd,"SALIR\n",sizeof("SALIR\n"),0); //Se manda el mensaje al servidor
	bzero(buffer,sizeof(buffer));
	recv(sd,buffer,sizeof(buffer),0); //Se recibe un mensaje del servidor

	std::cout <<std::endl <<buffer << std::endl;
	if (strcmp(buffer, "+Ok. Desconexion del cliente") !=0){
		send(sd,"SALIR\n",sizeof("SALIR\n"),0); //Se manda el mensaje al servidor
		bzero(buffer,sizeof(buffer));	
		recv(sd,buffer,sizeof(buffer),0); //Se recibe un mensaje del servidor

		std::cout  <<buffer << std::endl;
	}
	close(sd);
	exit(0);
}


