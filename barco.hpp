#ifndef BARCO_HPP
#define BARCO_HPP

#include <cstdlib>
#include <vector>

//Posibles posiciones del barco
#define POS_HORIZONTAL 0
#define POS_VERTICAL 1

//Estructura para indicar posiciones de los barcos en el tablero
struct posicion{
	int x; //Posicion con respecto al eje x
	int y; //Posicion con respecto al eje y
};


class Barco{

	private:
		int _largo; //Longitud del barco
		int _pos; //Orientacion del barco
		std::vector <struct posicion> _posiciones; //Posiciones que ocupan todas las partes del barco
		int _tocados; //Contador que indica el numero de partes sin tocar del barco
		

	public:
		//Constructor del barco
		Barco(int largo){

			_largo=largo;
			_pos=rand()%2;
			_tocados=largo;

			_posiciones.resize(largo);

		}

		//Devuelve la longitud del barco
		int getLargo(){

			return _largo;
		}

		//Devuelve la orientacion del barco
		int getPos(){

			return _pos;
		}

		//Devuelve el contador de partes no tocadas
		int getTocado(){

			return _tocados;
		}

		//Modifica el contador de partes no tocadas
		void setTocado(int tocados){

			 _tocados=tocados;
		}
		
		//Indica que el barco ha sido tocado en una ocasion
		void tocado(){
			if(getTocado() > 0) //El barco no esta hundido
				setTocado(getTocado()-1); //El barco ha sido tocado una vez
		}
		
		//Indica si el barco esta undido
		bool hundido(){

			if(getTocado()==0) //El barco esta hundido
				return true;
			else //El barco no esta hundido
				return false;
		}

		//Establece la posicion de una parte del barco
		void setPosicion(int i, int x, int y){

			_posiciones[i].x=x; //Se modifica la posicion con respecto al eje x
			_posiciones[i].y=y; //Se modifica la posicion con respecto al eje y

		}

		//Devuelve la posicion de una parte del barco
		struct posicion getPosicion(int i){

			return _posiciones[i];
		 }

		//Comprueba si el barco ocupa una posicion determinada
		bool tienePosicion(int x, int y){

			for(int i=0; i< getLargo(); i++){ //Se comprueban todas las partes del barco
				if(getPosicion(i).x == x and getPosicion(i).y == y) //Una parte del barco se situa en las coordenadas dadas
					return true;		
			}

			return false;
		}

		//Coloca el barco en el tablero
		void grabarPosiciones( int x, int y){


			for(int i=0; i<getLargo(); i++){ //Se colocan todas las partes del barco

				if(getPos()==POS_HORIZONTAL) //El barco esta en posicion horizontal
					setPosicion(i, x, y+i);
				else //El barco esta en posicion vertical
					setPosicion(i, x+i, y);

			}

		}

};


#endif
