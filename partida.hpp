#ifndef PARTIDA_HPP
#define PARTIDA_HPP

#include "usuario.hpp"
#include "tablero.hpp"

class Partida{


	private:
	
	Usuario _usuario1; //Jugador1
	Usuario _usuario2; //Jugador2

	//Tableros del usuario 1
	Tablero _t1contrario;
	Tablero _t1propio;

	//Tableros del usuario 2;
	Tablero _t2contrario;
	Tablero _t2propio;

	int _turno; //Solo tomará los valores 1 y 2 (diferenciará cuando le toca a cada usuario)

	int _booms1; //Disparos acertados por el jugador1
	int _booms2; //Disparos acertados por el jugador2

	int _shoots1; //Disparos realizados por el jugador1
	int _shoots2; //Disparos realizados por el jugador2

	

	public:
	//Constructor de la partida
	Partida():_t1contrario(0), _t1propio(1), _t2contrario(0), _t2propio(1){

		_booms1=0;
		_booms2=0;
		_shoots1=0;
		_shoots2=0;
		_turno=1;

		inicializarTableros(); //Se establecen los valores de las casillas de los tableros de ambos jugadores

	}
	
	//Devuelve el numero del jugador al que le toca actuar
	int getTurno(){

		return _turno;
	}

	//Cambia de turno
	void changeTurno(){
		if (getTurno()==1) //Es el turno del jugador1
			_turno=2;
		else //Es el turno del jugador2
			_turno=1;
	
	}

	//Devuelve al jugador1
	Usuario &getUsuario1(){

		return _usuario1;
	}

	//Devuelve al jugador2
	Usuario &getUsuario2(){

		return _usuario2;
	}

	//Establece quien es el jugador1
	void setUsuario1(Usuario usuario1){
		_usuario1=usuario1;
	}

	//Establece quien es el jugador2
	void setUsuario2(Usuario usuario2){
		_usuario2=usuario2;
	}

	Tablero getTablero1Propio(){
		return _t1propio;
	}

	Tablero getTablero1Contrario(){
		return _t1contrario;
	}

	Tablero getTablero2Propio(){
		return _t2propio;
	}

	Tablero getTablero2Contrario(){
		return _t2contrario;
	}

	//Devuelve el numero de aciertos del jugador1
	int getBooms1(){
		return _booms1;
	}

	//Devuelve el numero de aciertos del jugador2
	int getBooms2(){
		return _booms2;
	}		

	//Devuelve el numero de disparos del jugador1
	int getShoots1(){
		return _shoots1;
	}

	//Devuelve el numero de disparos del jugador2
	int getShoots2(){
		return _shoots2;
	}

	//Indica que el jugador1 ha realizado un disparo
	void shoots1(){
		_shoots1++;
	}

	//Indica que el jugador2 ha realizado un disparo
	void shoots2(){
		_shoots2++;
	}

	//Indica que el jugador1 ha acertado un disparo
	void acierto1(){
		_booms1++;
	}

	//Indica que el jugador2 ha acertado un disparo
	void acierto2(){
		_booms2++;
	}
	
	//Establece los valores iniciales de las casillas de los tableros de ambos jugadores
	void inicializarTableros(){
		_t1propio.inicializarTablero(); //Se inicializa el tablero del jugador1
		_t2propio.inicializarTablero(); //Se inicializa el tablero del jugador2
	}

	bool disparo(int x, char y); //Simula un disparo

};

#endif
