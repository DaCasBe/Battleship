#include "funcionesServidor.hpp"
#include "partida.hpp"
#include "tablero.hpp"
#include "usuario.hpp"
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


#define CONECTADO -1
#define NOMBRE 0
#define LOGUEADO 1
#define EN_COLA 2
#define EN_PARTIDA 3
#define MSG_SIZE 250


std::vector <std::string> dividirCadena(std::string cadena,std::string separador){
	std::vector <std::string> dividido;
	
	while(cadena.find(separador)<=MSG_SIZE){
		dividido.push_back(cadena.substr(0, cadena.find(separador)));
		cadena.erase(0, cadena.find(separador)+1);
	}

	dividido.push_back(cadena.substr(0, cadena.size()));
	
	return dividido;
}



bool leerUsuario(char buffer[MSG_SIZE],std::vector <Usuario> &usuarios,int descriptor){
	char nombre[100];
	char usuario2[100];
	std::string password;
	std::ifstream fichero;
	int i;
	char buffer2[MSG_SIZE];
	
	sscanf(buffer,"USUARIO %s",nombre); //Se lee el nombre de usuario
	
	if(nombre==NULL){ //No se ha introducido ningun nombre de usuario
		//Se dice al usuario que no introdujo ningun nombre de usuario
		bzero(buffer2,sizeof(buffer2));
		sprintf(buffer2,"-Err. No se introdujo el nombre de usuario\n");
		send(descriptor,buffer2,sizeof(buffer2),0);
		
		return false;
	}else{ //Se ha introducido un nombre de usuario
		for(i=0;i< (int)usuarios.size();i++){ //Se comprueba si existe un usuario conectado con ese nombre
			if(usuarios[i].getUsuario()==nombre){ //Existe un usuario conectado con ese nombre
				//Se dice al usuario que ya hay alguien conectado con ese nombre
				bzero(buffer2,sizeof(buffer2));
				sprintf(buffer2,"-Err. Ya existe un usuario con ese nombre\n");
				send(descriptor,buffer2,sizeof(buffer2),0);
											
				return false;
			}
		}
		
		fichero.open("usuarios.txt"); //Se abre el fichero para buscar el usuario
		
		if(!fichero){ //No se ha podido abrir el fichero
			//Se dice al usuario que no se ha podido abrir el fichero
			bzero(buffer2,sizeof(buffer2));
			sprintf(buffer2,"-Err. No se pudo abrir el fichero de usuarios\n");
			send(descriptor,buffer2,sizeof(buffer2),0);
			
			return false;
	
		}else{ //Se ha podido abrir el fichero
			while(fichero >> usuario2 >> password){ //Se leen los nombres de los usuarios
				if(strcmp(nombre,usuario2)==0){ //El usuario esta registrado
					//Se dice al usuario que se ha identificado correctamente
					bzero(buffer2,sizeof(buffer2));
					sprintf(buffer2,"+Ok. Usuario identificado correctamente\n");
					send(descriptor,buffer2,sizeof(buffer2),0);
					
					for(i=0;i<(int)usuarios.size();i++){
						if(usuarios[i].getSd()==descriptor){
							usuarios[i].setUsuario(nombre); //Se agrega al usuario al vector de usuarios
							usuarios[i].setEstado(NOMBRE); //Se especifica que el usuario ha introducido su nombre
							
							break;
						}
					}
				
					return true;
				}
			}
		
			fichero.close(); //Se cierra el fichero
		}
	}
	
	//Se dice al usuario que no esta registrado
	bzero(buffer2,sizeof(buffer2));
	sprintf(buffer2,"-Err. Usuario no encontrado\n");
	send(descriptor,buffer2,sizeof(buffer2),0);

	return false;
}

bool leerPassword(char buffer[MSG_SIZE],std::vector <Usuario> &usuarios,int descriptor){

	std::ifstream fichero;
	std::string nombre;
	char password[100];
	char password2[100];
	int i;
	char buffer2[MSG_SIZE];
	
	sscanf(buffer,"PASSWORD %s",password); //Se lee la password
	
	if(password==NULL){ //No se ha introducido ninguna password
		//Se dice al usuario que no ha introducido ninguna password
		bzero(buffer2,sizeof(buffer2));
		sprintf(buffer2,"-Err. No se introdujo la password\n");
		send(descriptor,buffer2,sizeof(buffer2),0);
	}
	
	for(i=0;i<(int)usuarios.size();i++){ //Se busca al usuario que ha introducido la password
		if(usuarios[i].getSd()==descriptor){ //Se ha encontrado al usuario que ha introducido la password
			if(usuarios[i].getEstado() != NOMBRE){ //Se comprueba que el usuario ha introducido su nombre anteriormente y que no esta ya logueado
				//Se dice al usuario que no ha introducido su nombre anteriormente o que no esta ya logueado
				bzero(buffer2,sizeof(buffer2));
				sprintf(buffer2,"-Err. No se introdujo el nombre previamente o el usuario ya fue logueado\n");
				send(descriptor,buffer2,sizeof(buffer2),0);
				
				return false;
			}
		}
	}

	fichero.open("usuarios.txt"); //Se abre el fichero de usuarios
	
	if(!fichero){ //No se ha podido abrir el fichero de usuarios
		//Se dice al usuario que no se ha podido abrir el fichero de usuarios
		bzero(buffer2,sizeof(buffer2));
		sprintf(buffer2,"-Err. No se pudo abrir el fichero de usuarios\n");
		send(descriptor,buffer2,sizeof(buffer2),0);
	}
	
	else{ //Se ha podido abrir el fichero de usuarios
		while(fichero >> nombre >> password2){ //Se leen los nombres de usuarios y las password
				for(i=0;i<(int)usuarios.size();i++){ //Se busca al usuario que ha introducido la password
					if(usuarios[i].getSd()==descriptor){ //Se ha encontrado al usuario que ha introducido la password
						std::string contrasenia=password;
						
						if (nombre==usuarios[i].getUsuario() and strcmp(password,password2)==0){ //La password corresponde con el usuario
							usuarios[i].setPassword(contrasenia); //Se asigna la password al usuario
							usuarios[i].setEstado(LOGUEADO); //Se especifica que el usuario ha introducido su password
							
							//Se dice al usuario que se ha logueado correctamente
							bzero(buffer2,sizeof(buffer2));
							sprintf(buffer2,"+Ok. Usuario logueado correctamente\n");
							send(descriptor,buffer2,sizeof(buffer2),0);
							return true;
						}/*else{ //La password no corresponde con el usuario
							//Se dice al usuario que la password es incorrecta
							bzero(buffer2,sizeof(buffer2));
							sprintf(buffer2,"-Err. Password incorrecta\n");
							send(descriptor,buffer2,sizeof(buffer2),0);
							return false;
						}*/
					}
				}
		}
		fichero.close(); //Se cierra el fichero
	}
	
	//Se dice al usuario que la password es incorrecta
	bzero(buffer2,sizeof(buffer2));
	sprintf(buffer2,"-Err. La password es incorrecta\n");
	send(descriptor,buffer2,sizeof(buffer2),0);
	
	return false;
}

void salirCliente(int socket, fd_set * readfds,std::vector <Usuario> &usuarios, std::vector <Usuario> &cola_espera, std::vector <Partida> &partidas){
  
	char buffer[250];
	int j;
	for (j=0; j<(int)usuarios.size(); j++){

		if(usuarios[j].getSd() == socket)
			break;

	}

	if(usuarios[j].getEstado() <= LOGUEADO){
		
		//Se dice al usuario que su desconexion ha sido correcta
		bzero(buffer,sizeof(buffer));
		sprintf(buffer,"+Ok. Desconexion del cliente");
    
		send(usuarios[j].getSd(),buffer,strlen(buffer),0);
		close(socket); //Se cierra el socket del cliente que sale
		FD_CLR(socket,readfds);
		usuarios.erase(usuarios.begin()+j);

	}else if (usuarios[j].getEstado() == EN_COLA){

		for(int k=0; k < (int)cola_espera.size(); k++){
			if(cola_espera[k].getSd()==socket){
				cola_espera.erase(cola_espera.begin()+k);	
				break;
			}
		}

		usuarios[j].setEstado(LOGUEADO);
		bzero(buffer,sizeof(buffer));
		sprintf(buffer,"+Ok. Se ha salido de la cola de espera\n");
		send(usuarios[j].getSd(),buffer,strlen(buffer),0);


	}else{
		int k;
		for(k=0; k < (int)partidas.size(); k++){
			if(partidas[k].getUsuario1().getSd()==socket){
				bzero(buffer,sizeof(buffer));
				sprintf(buffer,"+Ok. Se ha salido de la partida\n");
				send(partidas[k].getUsuario1().getSd(),buffer,strlen(buffer),0);

				bzero(buffer,sizeof(buffer));
				sprintf(buffer,"+Ok. Tu oponente ha abandonado la partida\n");
				send(partidas[k].getUsuario2().getSd(),buffer,strlen(buffer),0);
				break;
			}else if(partidas[k].getUsuario2().getSd()==socket){
				bzero(buffer,sizeof(buffer));
				sprintf(buffer,"+Ok. Se ha salido de la partida\n");
				send(partidas[k].getUsuario2().getSd(),buffer,strlen(buffer),0);

				bzero(buffer,sizeof(buffer));
				sprintf(buffer,"+Ok. Tu oponente ha abandonado la partida\n");
				send(partidas[k].getUsuario1().getSd(),buffer,strlen(buffer),0);
				break;
			}
		}


		int sacar=0;
		for (int cont=0; cont < (int)usuarios.size(); cont++){ //Se busca a los usuarios de la partida
			if(partidas[k].getUsuario1().getSd()==usuarios[cont].getSd()){ //Se ha encontrado al jugador1
				usuarios[cont].setEstado(LOGUEADO); //Se especifica que el jugador1 no esta ya en partida
				sacar++;
			}
			if(partidas[k].getUsuario2().getSd()==usuarios[cont].getSd()){ //Se ha encontrado al jugador2
				usuarios[cont].setEstado(LOGUEADO); //Se especifica que el jugador2 no esta ya en la partida
				sacar++;
			}
			if(sacar == 2){ //Se ha sacado a los 2 jugadores de la partida
				partidas.erase(partidas.begin()+k); //Se borra la partida
				break;
			}
		}
	}
}

bool registro(char buffer[MSG_SIZE],int descriptor){

	
	std::ofstream escribir;
	std::ifstream leer;
	char nombre[100];
	char password[100];
	char nombre2[100];
	char password2[100];
	char buffer2[MSG_SIZE];
	
	sscanf(buffer,"REGISTRO -u %s -p %s",nombre,password); //Se leen el nombre de usuario y la password
	
	if(nombre==NULL or password==NULL){ //Se comprueba que se han introducido un nombre de usuario y una password
		//Se dice al usuario que no se introdujo el nombre de usuario o la password
		bzero(buffer2,sizeof(buffer2));
		sprintf(buffer2,"-Err. No se introdujo el nombre de usuario o la password\n");
		send(descriptor,buffer2,sizeof(buffer2),0);
		
		return false;
	}
	
	leer.open("usuarios.txt"); //Se abre el fichero de usuarios para leer

	if(!leer){ //No se ha podido abrir el fichero de usuarios
		//Se crea el fichero de usuarios
		escribir.open("usuarios.txt", std::ios::out);
		escribir.close();	
	}
	
	else{ //Se ha podido abrir el fichero de usuarios
		while(leer >> nombre2 >> password2){ //Se leen los nombres de usuarios
			if(strcmp(nombre,nombre2)==0){ //Se comprueba si el nombre de usuario esta ya registrado
				//Se dice al usuario que el nombre ya esta registrado
				bzero(buffer2,sizeof(buffer2));
				sprintf(buffer2,"-Err. Ya existe el usuario\n");
				send(descriptor,buffer2,sizeof(buffer2),0);
				
				return false;
			}
		}
	}	
	leer.close(); //Se cierra el fichero de usuarios
	
	escribir.open("usuarios.txt", std::ios::app); //Se abre el fichero de usuarios para escribir
		
	if(!escribir){ //No se ha podido abrir el fichero de usuarios
		//Se dice al usuario que no se ha podido abrir el fichero de usuarios
		bzero(buffer2,sizeof(buffer2));
		sprintf(buffer2,"-Err. No se pudo abrir el fichero de usuarios\n");
		send(descriptor,buffer2,sizeof(buffer2),0);
		return false;
	}else{
		escribir << nombre << " " << password << std::endl; //Escribimos en el fichero el nombre y la password del usuario

		escribir.close(); //Cerramos el fichero de usuarios
	}
	
	//Se dice al usuario que ha sido registrado correctamente
	bzero(buffer2,sizeof(buffer2));
	sprintf(buffer2,"+Ok. Usuario registrado correctamente\n");
	send(descriptor,buffer2,sizeof(buffer2),0);
	
	return true;
}


void deMatrizaVector(std::vector < std::vector <char> > matriz, std::string &vector){
	int i;
	int j;
	int k=0;
	
	for(i=0;i<(int)matriz.size();i++){
		for(j=0;j<(int)matriz[i].size();j++){
			vector.push_back(matriz[i][j]);
			k++;
			
			if((k+1)%20==0){
				if (i!=9)
				vector.push_back(';');
			}else{
				vector.push_back(',');
			}
			
			k++;
		}
	}
	
}
/*
void deMatrizaVector(std::vector < std::vector <char> > matriz,char vector[]){
	int i;
	int j;
	int k=0;
	
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			vector[k]=matriz[i][j];
			k++;
			
			if((k+1)%20==0){
				vector[k]=';';
			}
			
			else{
				vector[k]=',';
			}
			
			k++;
		}
	}
}*/



void enviarTableros(Partida p){

	char buffer2[MSG_SIZE];

	std::string tablero1Propio;
	
	deMatrizaVector(p.getTablero1Propio().getMatriz(),tablero1Propio);
	bzero(buffer2,sizeof(buffer2));
	strcpy(buffer2,tablero1Propio.c_str());
	send(p.getUsuario1().getSd(),buffer2,sizeof(buffer2),0);
														
	std::string tablero1Contrario;
	deMatrizaVector(p.getTablero1Contrario().getMatriz(),tablero1Contrario);
	bzero(buffer2,sizeof(buffer2));
	strcpy(buffer2,tablero1Contrario.c_str());
	send(p.getUsuario1().getSd(),buffer2,sizeof(buffer2),0);
														
	std::string tablero2Propio;
	deMatrizaVector(p.getTablero2Propio().getMatriz(),tablero2Propio);
	bzero(buffer2,sizeof(buffer2));
	strcpy(buffer2,tablero2Propio.c_str());
	send(p.getUsuario2().getSd(),buffer2,sizeof(buffer2),0);
														
	std::string tablero2Contrario;
	deMatrizaVector(p.getTablero2Contrario().getMatriz(),tablero2Contrario);
	bzero(buffer2,sizeof(buffer2));
	strcpy(buffer2,tablero2Contrario.c_str());
	send(p.getUsuario2().getSd(),buffer2,sizeof(buffer2),0);
}


void crearPartida(std::vector <Usuario> &usuarios, std::vector <Usuario> &cola_espera, std::vector <Partida> &partidas){

Partida p;
int cont;	
char buffer[MSG_SIZE];
std::string nombre_primero;
											
	for (cont=0; cont< (int)usuarios.size(); cont++){ //Se busca al usuario que ha mandado el mensaje
		if(usuarios[cont].getSd()==cola_espera[0].getSd()){ //Se ha identificado al usuario que ha mandado el mensaje
			usuarios[cont].setEstado(EN_PARTIDA); //Se especifica que el usuario esta en partida
																
			break;
		}
	}
	
	nombre_primero=usuarios[cont].getUsuario();
																
	p.setUsuario1(cola_espera[0]); //Se agrega a la partida al primer usuario de la cola de espera
														
	//Se dice al usuario que se ha encontrado una partida
	bzero(buffer, sizeof(buffer));
	sprintf(buffer,"+Ok. Se ha encontrado una partida\n");
	send(usuarios[cont].getSd(),buffer,sizeof(buffer),0);
														
	cola_espera.erase(cola_espera.begin()); //Se saca al primer usuario de la cola de espera
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer,"+Ok. Es el turno de %s\n",nombre_primero.c_str());
	send(usuarios[cont].getSd(),buffer,sizeof(buffer),0);

	for (cont=0; cont< (int)usuarios.size(); cont++){ //Se busca al siguiente en la cola de usuarios
		if(usuarios[cont].getSd()==cola_espera[0].getSd()){ //Se ha identificado al siguiente en la cola de usuarios
			usuarios[cont].setEstado(EN_PARTIDA); //Se especifica que el usuario identificado esta en partida
			break;
		}
	}

	p.setUsuario2(cola_espera[0]); //Se agrega al usuario a la partida
														
	//Se dice al usuario que se ha encontrado una partida
	bzero(buffer, sizeof(buffer));
	sprintf(buffer,"+Ok. Se ha encontrado una partida\n");
	send(usuarios[cont].getSd(),buffer,sizeof(buffer),0);
														
	cola_espera.erase(cola_espera.begin()); //Se saca al usuario de la cola de espera
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer,"+Ok. Es el turno de %s\n",nombre_primero.c_str());
	send(usuarios[cont].getSd(),buffer,sizeof(buffer),0);
	
	partidas.push_back(p); //Se agrega la partida al vector de partidas
														
	enviarTableros(p);


}
