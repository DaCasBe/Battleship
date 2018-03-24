#ifndef TABLERO_HPP
#define TABLERO_HPP

#include <iostream>
#include <vector>
#include "barco.hpp"
#include "macros.hpp"

//Posibles tipos de tablero
#define TIPO_OCULTO 0
#define TIPO_DESCUBIERTO 1

class Tablero{

	private:
		std::vector < std::vector <char> > _matriz; //Tablero
		int _tipo; //Tipo de tablero
		int _size;
		std::vector <Barco> _barcos; //Barcos situados en el tablero

	public:
		//Constructor del tablero
		Tablero(int tipo){

			_matriz.resize(10); //Se crea el tablero
			
			if(tipo==TIPO_OCULTO){ //El tablero oculta la informacion
				for (int i=0; i<10; i++)
					_matriz[i].resize(10, 'X');
			}

			if(tipo==TIPO_DESCUBIERTO){ //El tablero permite ver la informacion
				for (int i=0; i<10; i++)
					_matriz[i].resize(10, 'A');
			}

			_tipo=tipo; //Se establece el tipo del tablero
			_size=10;
		}
		
		//Devuelve el tablero
		std::vector < std::vector <char> > getMatriz(){
			return _matriz;
		}

		//Devuelve una posicion del tablero
		char getPosicion(int x, int y){

			return _matriz[x][y];
		}

		//Devuelve un barco
		Barco &getBarco(int i){

			return _barcos[i];	
		}

		//Modifica el valor de una posicion del tablero
		void setMatriz(int x, int y, char v){

			_matriz[x][y]=v;

		}

		//Coloca un barco en el tablero
		void setBarco(Barco b, int x, int y){
					

			if(b.getPos() == POS_HORIZONTAL){ //El barco esta en posicion horizontal

				for (int i=0; i<b.getLargo(); i++){

					setMatriz(x, y, 'B'); //Se coloca una parte del barco
					y++;
				}
	
			}else{ //El barco esta en posicion vertical
				for (int i=0; i<b.getLargo(); i++){

					setMatriz(x, y, 'B'); //Se coloca una parte del barco
					x++;

				}
			}

			_barcos.push_back(b); //Se agrega el barco al vector de barcos
		}

		//Muestra el tablero por la pantalla
		void imprimeTablero(){
			std::cout << "     A   B   C   D   E   F   G   H   I   J" << std::endl;
			for(int i=0; i<10; i++){
				if(i<9)
					std::cout << i+1 << "  ";
				else
					std::cout << i+1 << " ";
		
				for (int j=0; j<10; j++){
					if(getPosicion(i,j)=='B'){
						std::cout << "| "<< IGREEN << getPosicion(i, j) << RESET << " ";
					}
					
					else if(getPosicion(i,j)=='A'){
						std::cout << "| " << IBLUE << getPosicion(i,j) << RESET << " ";
					}
					
					else{
						std::cout << "| "<< getPosicion(i, j) << " ";
					}
				  }
				std::cout <<"|" << std::endl;
			}
		}

		
		int disparado(int x, int y){

			int i;
			for(i=0; i<(int)_barcos.size(); i++){
				if(getBarco(i).tienePosicion(x, y)){
					getBarco(i).tocado();
					
					break;


				}
			}
			if(getBarco(i).hundido())
				return 0;	
			else
				return 1;
		}

		//Establece los valores iniciales de las casillas del tablero
		void inicializarTablero();
		
		//Coloca un barco en el tablero
		void generarPosicion(int &x, int &y, Barco &b);
		
		//Comprueba si se puede colocar un barco en una posicion
		bool comprobarPosicion(int x, int y, Barco b);
/*
		void localizarBarcos(){

			for (int i=0; i < _barcos.size(); i++){
					std::cout << getBarco(i).getLargo()<<": ";
				for(int j=0; j<getBarco(i).getLargo(); j++)
					std::cout << getBarco(i).getPosicion(j).x << ", "<<getBarco(i).getPosicion(j).y << "   ";
			
				std::cout << std::endl;
			}

		}

		~Tablero(){
			for (int i=0; i<10; i++)
				_matriz[i].resize(0);

			_matriz.resize(0);
			//_barcos.resize(0);
		}*/
};

#endif
