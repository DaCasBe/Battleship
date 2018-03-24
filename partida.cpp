#include "partida.hpp"
#include <sys/socket.h>
#include <string.h>
#include <cstdio>

#define MSG_SIZE 250 //Capacidad maxima del buffer usado para enviar y recibir mensajes

//Simula un disparo
bool Partida:: disparo(int x, char y){

	int ascii;
	char buffer[MSG_SIZE];
	
	x--; //Restamos 1 a x, ya que aunque en el juego el tablero es de 1 a 10, esta implementado para ir de 0 a 9
	ascii=y-65;
	


	if(getTurno()==1){ //Es el turno del jugador1
		switch(_t1contrario.getPosicion(x, ascii)){ //Se comprueba el valor de la casilla disparada	

			case 'A': //El valor es 'A'
			case 'B': //El valor es 'B'
				//Se dice al usuario que esa casilla ya ha sido disparada anteriormente
				bzero(buffer, sizeof(buffer));
				sprintf(buffer,"-Err. Esa casilla ya ha sido disparada\n");
				send(getUsuario1().getSd(),buffer,sizeof(buffer),0);
				return false;
				break;
			case 'X': //El valor es 'X'

				bzero(buffer, sizeof(buffer));
				sprintf(buffer,"+Ok. Disparo en %c,%i\n", y, x+1);
				send(getUsuario2().getSd(),buffer,sizeof(buffer),0);

				switch(_t2propio.getPosicion(x, ascii)){ //Se comprueba el valor real de la casilla

					case 'A': //El valor es 'A'
						//Se dice al jugador que ha disparado al agua
						bzero(buffer, sizeof(buffer));
						sprintf(buffer,"+Ok. AGUA: %c,%i\n", y, x+1);
						send(getUsuario1().getSd(),buffer,sizeof(buffer),0);
						
						_t1contrario.setMatriz( x, ascii, 'A'); //Se modifica el valor de la casilla en el tablero del jugador
						_t2propio.setMatriz(x,ascii,'X'); //Se modifica el valor de la casilla en el tablero del jugador contrario
						
						changeTurno(); //Se cambia el turno
						//Se dice a ambos jugadores a quien le toca disparar
						bzero(buffer, sizeof(buffer));
						sprintf(buffer,"+Ok. Es el turno de %s\n",getUsuario2().getUsuario().c_str());
						send(getUsuario1().getSd(),buffer,sizeof(buffer),0);
						send(getUsuario2().getSd(),buffer,sizeof(buffer),0);
						break;

					case 'B': //El valor es 'B'
						_t1contrario.setMatriz( x, ascii, 'B'); //Se modifica el valor de la casilla en el tablero del jugador
						_t2propio.setMatriz(x,ascii,'X');
						acierto1(); //El jugador1 ha acertado el disparo
						if(_t2propio.disparado(x, ascii)==0){ //El barco disparado se ha hundido
							//Se dice al jugador que ha hundido un barco
							bzero(buffer, sizeof(buffer));
							sprintf(buffer,"+Ok. HUNDIDO: %c,%i\n", y, x+1);
							send(getUsuario1().getSd(),buffer,sizeof(buffer),0);
							
							//Se dice a ambos jugadores a quien le toca disparar
							bzero(buffer, sizeof(buffer));
							sprintf(buffer,"+Ok. Es el turno de %s\n",getUsuario1().getUsuario().c_str());
							send(getUsuario1().getSd(),buffer,sizeof(buffer),0);
							send(getUsuario2().getSd(),buffer,sizeof(buffer),0);
						}else{ //El barco disparado no se ha hundido
							//Se dice al jugador que ha acertado a un barco
							bzero(buffer, sizeof(buffer));
							sprintf(buffer,"+Ok. TOCADO: %c,%i\n", y, x+1);
							send(getUsuario1().getSd(),buffer,sizeof(buffer),0);
							
							//Se dice a ambos jugadores a quien le toca disparar
							bzero(buffer, sizeof(buffer));
							sprintf(buffer,"+Ok. Es el turno de %s\n",getUsuario1().getUsuario().c_str());
							send(getUsuario1().getSd(),buffer,sizeof(buffer),0);
							send(getUsuario2().getSd(),buffer,sizeof(buffer),0);
						}

				};

		};
	shoots1(); //El jugador1 ha realizado un disparo

	return true;

	}else{ //Es el turno del jugador2
		switch(_t2contrario.getPosicion(x, ascii)){ //Se comprueba el valor de la casilla	

		case 'A': //El valor es 'A'
		case 'B': //El valor es 'B'
			//Se dice al jugador que esa casilla ya ha sido disparada anteriormente
			bzero(buffer, sizeof(buffer));
			sprintf(buffer,"-Err. Esa casilla ya ha sido disparada\n");
			send(getUsuario2().getSd(),buffer,sizeof(buffer),0);
			return false;
			break;
		case 'X': //El valor es 'X'

				bzero(buffer, sizeof(buffer));
				sprintf(buffer,"+Ok. Disparo en %c,%i\n", y, x+1);
				send(getUsuario1().getSd(),buffer,sizeof(buffer),0);

			switch(_t1propio.getPosicion(x, ascii)){ //Se comprueba el valor real de la casilla

				case 'A': //El valor es 'A'
					//Se dice al jugador que ha disparado al agua
					bzero(buffer, sizeof(buffer));
					sprintf(buffer,"+Ok. AGUA: %c,%i\n", y, x+1);
					send(getUsuario2().getSd(),buffer,sizeof(buffer),0);
					_t2contrario.setMatriz( x, ascii, 'A'); //Se modifica el valor de la casilla en el tablero del jugador
					_t1propio.setMatriz(x,ascii,'X'); //Se modifica el valor de la casilla en el tablero del jugador contrario
					changeTurno(); //Se cambia el turno
					//Se dice a ambos jugadores a quien le toca disparar
					bzero(buffer, sizeof(buffer));
					sprintf(buffer,"+Ok. Es el turno de %s\n",getUsuario1().getUsuario().c_str());
					send(getUsuario1().getSd(),buffer,sizeof(buffer),0);
					send(getUsuario2().getSd(),buffer,sizeof(buffer),0);
					
					break;

				case 'B':
					_t2contrario.setMatriz( x, ascii, 'B'); //Se modifica el valor de la casilla en el tablero del jugador
					_t1propio.setMatriz(x,ascii,'X');
					acierto2(); //El jugador2 ha acertado el disparo
					if(_t2propio.disparado(x, ascii)==0){ //El barco disparado se ha hundido
						//Se dice al jugador que ha hundido un barco
						bzero(buffer, sizeof(buffer));
						sprintf(buffer,"+Ok. HUNDIDO: %c,%i\n", y, x+1);
						send(getUsuario2().getSd(),buffer,sizeof(buffer),0);
						
						//Se dice a ambos jugadores a quien le toca disparar
						bzero(buffer, sizeof(buffer));
						sprintf(buffer,"+Ok. Es el turno de %s\n",getUsuario2().getUsuario().c_str());
						send(getUsuario1().getSd(),buffer,sizeof(buffer),0);
						send(getUsuario2().getSd(),buffer,sizeof(buffer),0);
					}
					
					else{ //El barco disparado no se ha hundido
						//Se dice al jugador que ha acertado a un barco
						bzero(buffer, sizeof(buffer));
						sprintf(buffer,"+Ok. TOCADO: %c,%i\n", y, x+1);
						send(getUsuario2().getSd(),buffer,sizeof(buffer),0);
						
						//Se dice a ambos jugadores a quien le toca disparar
						bzero(buffer, sizeof(buffer));
						sprintf(buffer,"+Ok. Es el turno de %s\n",getUsuario2().getUsuario().c_str());
						send(getUsuario1().getSd(),buffer,sizeof(buffer),0);
						send(getUsuario2().getSd(),buffer,sizeof(buffer),0);
					}

				};

		};
		shoots2(); //El jugador2 ha realizado un disparo


		return true;
	}


}

