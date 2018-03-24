#include <iostream>
#include <vector>
#include <fstream>

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include "partida.hpp"
#include "tablero.hpp"
#include "usuario.hpp"
#include "funcionesServidor.hpp"

#define CONECTADO -1
#define NOMBRE 0
#define LOGUEADO 1
#define EN_COLA 2
#define EN_PARTIDA 3

#define MSG_SIZE 250
#define MAX_CLIENTS 30
#define MAX_PARTIDAS 10

std::vector <Usuario> usuarios;
int sd;
fd_set readfds;


void manejador (int signum);

int main ( ){
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int new_sd;
	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE];
	socklen_t from_len;

	fd_set auxfds;
	int salida;

	int i,j,cont2,cont5;
	int recibidos;
	//char identificador[MSG_SIZE];
	std::vector <Usuario> cola_espera;
	std::vector <Partida> partidas;
	int on;
	int x;
	char y;
	
	srand(time(NULL));
    
    
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}
    
    // Activaremos una propiedad del socket que permitir· que otros
    // sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    // Esto permitir· en protocolos como el TCP, poder ejecutar un
    // mismo programa varias veces seguidas y enlazarlo siempre al
    // mismo puerto. De lo contrario habria que esperar a que el puerto
    // quedase disponible (TIME_WAIT en el caso de TCP)
    on=1;
    setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));



	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2050);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}
	

   	/*---------------------------------------------------------------------
		Del las peticiones que vamos a aceptar sólo necesitamos el 
		tamaño de su estructura, el resto de información (familia, puerto, 
		ip), nos la proporcionará el método que recibe las peticiones.
   	----------------------------------------------------------------------*/
	from_len = sizeof (from);


	if(listen(sd,30) == -1){ //Se habilita el socket para recibir conexiones
		perror("Error en la operación de listen");
		exit(1);
	}
    
    //Inicializar los conjuntos fd_set
    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);

	//Capturamos la señal SIGINT (Ctrl+c)
	signal(SIGINT,manejador);

	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
	while(1){
            
		//Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)
		auxfds = readfds;
		salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);
            
		if(salida > 0){                              
			for(i=0; i<FD_SETSIZE; i++){ //Buscamos el socket por el que se ha establecido la comunicación
				if(FD_ISSET(i, &auxfds)){
					if(i == sd){ //Encontramos el socket
						if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){ //No pueden aceptarse conexiones
							perror("Error aceptando peticiones");
						}else{ //Se aceptan conexiones
							if((int)usuarios.size() < MAX_CLIENTS){ //Aun no se ha llegado al limite de usuarios conectados
								Usuario u;
								u.setSd(new_sd);
								usuarios.push_back(u);
								
								FD_SET(new_sd,&readfds);
								
								//Se le dice al usuario que se ha conectado
								strcpy(buffer, "+Ok. Usuario conectado\n");
								send(new_sd,buffer,strlen(buffer),0);                                
							}else{ //Se ha llegado al limite de usuarios conectados
							//Se le dice al usuario que no puede conectarse porque hay demasiados usuarios conectados
                        	          	 bzero(buffer,sizeof(buffer));
                    	                strcpy(buffer,"-Err. Demasiados clientes conectados\n");
                    	                send(new_sd,buffer,strlen(buffer),0);
                    	                close(new_sd); //Se cierra el socket
							}                                
                        	   	}
					}else if (i == 0){ //Se ha introducido información de teclado
						bzero(buffer, sizeof(buffer));
						fgets(buffer, sizeof(buffer),stdin);
                            	
						if(strcmp(buffer,"SALIR\n") == 0){ //Controlar si se ha introducido "SALIR"
							for (j = 0; j < (int)usuarios.size(); j++){ //Se les dice a los usuarios que el servidor se ha desconectado y se les echa
								send(usuarios[j].getSd(), "+Ok. Desconexion del servidor", strlen("+Ok. Desconexion del servidor\n"),0);
								close(usuarios[j].getSd());
								FD_CLR(usuarios[j].getSd(),&readfds);
							}
                                    close(sd); //Se cierra el socket
                                    exit(-1);                                
                            	}
                            
					}else{
						bzero(buffer,sizeof(buffer));
						recibidos = recv(i,buffer,sizeof(buffer),0); //Se recibe un mensaje

						if(recibidos > 0){ //Realmente se ha recibido un mensaje
							if(strcmp(buffer,"SALIR\n") == 0){ //El mensaje recibido es "SALIR"
								salirCliente(i,&readfds,usuarios, cola_espera, partidas); //Se saca al cliente
							}else{ //El mensaje recibido no es "SALIR"
                                    	
           						std::vector<std::string> splitted_buffer = dividirCadena(std::string(buffer), " "); //Se analiza el mensaje
								if(splitted_buffer[0] == "USUARIO"){ //El mensaje recibido es "USUARIO"
									for(cont5=0;cont5<(int)usuarios.size();cont5++){ //Se busca al usuario que ha mandado el mensaje
										if(usuarios[cont5].getSd()==i){ //Se encuentra al usuario que ha mandado el mensaje
											break;
										}
									}
									
									if(usuarios[cont5].getEstado()>=LOGUEADO){ //El usuario ya esta logueado
										//Se dice al usuario que no puede loguearse porque ya esta logueado
										bzero(buffer, sizeof(buffer));
										sprintf(buffer,"-Err. Ya esta logueado\n");
										send(i,buffer,sizeof(buffer),0);
									}
									
									else{ //El usuario no esta logueado
                      		        	leerUsuario(buffer,usuarios,i); //Se comprueba que el usuario es correcto
                      		        }

                         	 		}else if(splitted_buffer[0] == "PASSWORD"){ //El mensaje recibido es "PASSWORD"
										for(cont5=0;cont5<(int)usuarios.size();cont5++){ //Se busca al usuario que ha mandado el mensaje
											if(usuarios[cont5].getSd()==i){ //Se encuentra al usuario que ha mandado el mensaje
												break;
											}
										}
									
										if(usuarios[cont5].getEstado()>=LOGUEADO){ //El usuario ya esta logueado
											//Se dice al usuario que no puede loguearse porque ya esta logueado
											bzero(buffer, sizeof(buffer));
											sprintf(buffer,"-Err. Ya esta logueado\n");
											send(i,buffer,sizeof(buffer),0);
										}
									
										else{ //El usuario no esta logueado
                      		        		leerPassword(buffer,usuarios,i); //Se comprueba que la password es correcta
                      		       	 }

								}else if(splitted_buffer[0] == "REGISTRO"){ //El mensaje recibido es "REGISTRO"
									if(splitted_buffer.size() >= 4 and splitted_buffer[1] == "-u" and splitted_buffer[3] == "-p"){ //Se comprueba que se hayan introducido los comandos "-u" y "-p"
          	                        	registro(buffer,i); //Se registra al usuario
          	                        	
									}else{ //No se han introducido los comandos "-u" y "-p"
										//Se dice al usuario que no ha escrito la peticion de registro con el formato correcto
										bzero(buffer, sizeof(buffer));
										sprintf(buffer,"-Err. Formato incorrecto de REGISTRO\n");
										send(i,buffer,sizeof(buffer),0);
									}

								}else if (strcmp(buffer, "INICIAR-PARTIDA\n")==0){ //El mensaje recibido es "INICIAR-PARTIDA"
									for(int cont=0; cont<(int)usuarios.size();cont++){ //Se busca al usuario que ha mandado el mensaje
										if(usuarios[cont].getSd()==i){ //Se ha identificado al usuario que ha mandado el mensaje
											if(usuarios[cont].getEstado() != LOGUEADO){ //El usuario no esta logueado
												//Se dice al usuario que no puede iniciar una partida porque no esta logueado
												bzero(buffer, sizeof(buffer));
												sprintf(buffer,"-Err. No esta logueado\n");
												send(i,buffer,sizeof(buffer),0);
											}else{ //El usuario esta logueado
												//Se dice al usuario que se esta buscando una pareja para jugar
												bzero(buffer, sizeof(buffer));
												sprintf(buffer,"+Ok. Esperando partida...\n");
												send(i,buffer,sizeof(buffer),0);
												usuarios[cont].setEstado(EN_COLA); //Se especifica que el usuario esta en cola a la espera de jugar
												cola_espera.push_back(usuarios[cont]); //Se agrega al usuario a la cola de usuarios
												
												while(cola_espera.size()>1){ //Hay al menos un usuario en la cola de espera
													if(partidas.size()<MAX_PARTIDAS){ //No se ha superado el numero maximo de partidas simultaneas
														crearPartida(usuarios, cola_espera, partidas);

					 								}else{/* //Se ha superado el numero maximo de partidas simultaneas
					 									//Se dice al usuario que no puede jugar porque hay demasiadas partidas activas
														bzero(buffer, sizeof(buffer));
														sprintf(buffer,"-Err. Hay demasiadas partidas\n");
														send(i,buffer,sizeof(buffer),0);*/
														
														break;
													}
												}
											}
										}
									}
								}else if(splitted_buffer[0] == "DISPARO"){ //El mensaje recibido es "DISPARO"
									for(int cont=0; cont< (int)usuarios.size();cont++){ //Se busca al usuario que ha mandado el mensaje
										if(usuarios[cont].getSd()==i){ //Se identifica al usuario que ha mandado el mensaje
											if(usuarios[cont].getEstado() != EN_PARTIDA){ //El usuario no esta en partida
												//Se dice al usuario que no puede disparar porque no esta en partida
												bzero(buffer, sizeof(buffer));
												sprintf(buffer,"-Err. No esta en una partida\n");
												send(i,buffer,sizeof(buffer),0);
												break;
											}else{ //El usuario esta en partida
												sscanf(buffer,"DISPARO %c,%i", &y, &x); //Se leen las coordenadas del disparo
												if(y>='A' and y<='J' and x>=1 and x<=10){ //Las coordenadas son correctas
													for (cont2=0; cont2 < (int)partidas.size(); cont2++){
														if(partidas[cont2].getUsuario1().getSd() == i){ //El usuario es el jugador1
															if(partidas[cont2].getTurno()==1){ //Es el turno del jugador1
																partidas[cont2].disparo(x, y); //Se realiza el disparo
																enviarTableros(partidas[cont2]); //Se envian los tableros a los jugadores
																break;
															}else{ //No es el turno del jugador1
																//Se dice al usuario que no puede disparar porque no es su turno
																bzero(buffer, sizeof(buffer));
																sprintf(buffer,"-Err. Debe esperar su turno\n");
																send(i,buffer,sizeof(buffer),0);
																break;
															}
														}else if(partidas[cont2].getUsuario2().getSd() == i){ //El usuario es el jugador2
															if(partidas[cont2].getTurno()==2){ //Es el turno del jugador2
																partidas[cont2].disparo(x, y); //Se realiza el disparo
																enviarTableros(partidas[cont2]); //Se envian los tableros a los jugadores
																break;
															}else{ //No es el turno del jugador2
																//Se dice al usuario que no puede disparar porque no es su turno
																bzero(buffer, sizeof(buffer));
																sprintf(buffer,"-Err. Debe esperar su turno\n");
																send(i,buffer,sizeof(buffer),0);
																break;
															}
														}
													}
													if(partidas[cont2].getBooms1()==20){ //Ha ganado el jugador1
														//Se dice a ambos jugadores que ha ganado el jugador1
														
														bzero(buffer, sizeof(buffer));
														sprintf(buffer,"+Ok. %s ha ganado, numero de disparos: %d\n",partidas[cont2].getUsuario1().getUsuario().c_str(),partidas[cont2].getShoots1());
														send(partidas[cont2].getUsuario1().getSd(),buffer,sizeof(buffer),0);
														send(partidas[cont2].getUsuario2().getSd(),buffer,sizeof(buffer),0);
													}
													
													if(partidas[cont2].getBooms2()==20){ //Ha ganado el jugador2
														//Se dice a ambos jugadores que ha ganado el jugador2
														bzero(buffer, sizeof(buffer));
														sprintf(buffer,"+Ok. %s ha ganado, numero de disparos: %d\n",partidas[cont2].getUsuario2().getUsuario().c_str(),partidas[cont2].getShoots2());
														send(partidas[cont2].getUsuario2().getSd(),buffer,sizeof(buffer),0);
														send(partidas[cont2].getUsuario1().getSd(),buffer,sizeof(buffer),0);
													}
													
													if(partidas[cont2].getBooms1()==20 or partidas[cont2].getBooms2()==20){ //Se ha acabado la partida
														int sacar=0;
														for (int cont3=0; cont3 < (int)usuarios.size(); cont3++){ //Se busca a los usuarios de la partida
															if(partidas[cont2].getUsuario1().getSd()==usuarios[cont3].getSd()){ //Se ha encontrado al jugador1
																usuarios[cont3].setEstado(LOGUEADO); //Se especifica que el jugador1 no esta ya en partida
																sacar++;
															}
															if(partidas[cont2].getUsuario2().getSd()==usuarios[cont3].getSd()){ //Se ha encontrado al jugador2
																usuarios[cont3].setEstado(LOGUEADO); //Se especifica que el jugador2 no esta ya en la partida
																sacar++;
															}

															if(sacar == 2){ //Se ha sacado a los 2 jugadores de la partida
																partidas.erase(partidas.begin()+cont2); //Se borra la partida													
																if(partidas.size() == MAX_PARTIDAS-1 and cola_espera.size() > 1){
																	crearPartida(usuarios, cola_espera, partidas);
																}
																
																break;
															}
														}
													}
												}else{ //Las coordenadas no son correctas
													//Se dice al usuario que las coordenadas son incorrectas
													bzero(buffer, sizeof(buffer));
													sprintf(buffer,"-Err. No existe esa posicion\n");
													send(i,buffer,sizeof(buffer),0);
												} 
											}
										}
									}								
                           	    		}else{ //Se ha recibido otro mensaje
                           	    	//Se dice al usuario que el mensaje que ha mandado es incorrecto
									bzero(buffer, sizeof(buffer));
									sprintf(buffer,"-Err. Mensaje Incorrecto\n");
									send(i,buffer,sizeof(buffer),0);
								}
							}
						}
                            //Si el cliente introdujo ctrl+c
                            /* if(recibidos== 0)
                            {
                                printf("El socket %d, ha introducido ctrl+c\n", i);
                                //Eliminar ese socket
                                salirCliente(i,&readfds,&numClientes,usuarios);
                            }*/
					}
				}
			}
		}
	}
	close(sd); //Se cierra el socket

	return 0;
}

void manejador (int signum){

    signal(SIGINT,manejador);
    
    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
	for (int j = 0; j < (int)usuarios.size(); j++){ //Se les dice a los usuarios que el servidor se ha desconectado y se les echa
	send(usuarios[j].getSd(), "+Ok. Desconexion del servidor", strlen("+Ok. Desconexion del servidor\n"),0);
		close(usuarios[j].getSd());
		FD_CLR(usuarios[j].getSd(),&readfds);
	}
	close(sd); //Se cierra el socket
	exit(-1);                                
}
