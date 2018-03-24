#include "tablero.hpp"


//Comprueba si se puede colocar un barco en una posicion
bool Tablero:: comprobarPosicion(int x, int y, Barco b){


	int i, j;

	if(b.getPos()==POS_HORIZONTAL){ //El barco esta en posicion horizontal
		for(i=(x-1); i<=x+1; i++){ //Se comprueban tres bloques: la fila de arriba, la del barco y la de abajo
			//Se comprueba la longitud del barco, que es la longitud de los tres bloques a comparar
			//La central es mas larga, compararemos aparte los extremos
			for(j=y-1; j<y+b.getLargo()+1; j++){
				if(i!=-1 && i!=10 ){ //La casilla es valida con respecto al eje x
					if(j!=-1 && j!=10){ //La casilla es valida con respecto al eje y
						if(getPosicion(i, j)=='B') //Hay parte de un barco en la casilla a comprobar
						return false;
					}
	
				}
			}
		}
		
		return true;

	}else{ //El barco esta en posicion vertical
		for(j=(y-1); j<=y+1; j++){ //Se comprueban tres bloques: la columna de la izquierda, la del barco y la de la derecha
			//Se comprueba la longitud del barco, que es la longitud de los tres bloques a comparar
			//La central es mas larga, compararemos aparte los extremos
			for(i=x-1; i<x+b.getLargo()+1; i++){
				if(j!=-1 && j!=10){ //La casilla es valida con respecto al eje x
					if(i!=-1 && i!=10){ //La casilla es valida con respecto a eje y
						if(getPosicion(i, j)=='B') //Hay parte de un barco en la casilla a comprobar
							return false;
					}
				}
			}
		}
		
	return true;

	}

}

//Coloca un barco en el tablero
void Tablero::generarPosicion(int &x, int &y, Barco &b){

	int modulo=11-b.getLargo(); //Se establece un margen para colocar el barco (la longitud de los lados del tablero)
	
	//Se establece la posicion del barco de manera aleatoria
	x=rand()%modulo;
	y=rand()%modulo;

	bool disponible=comprobarPosicion(x, y, b); //Se comprueba que la posicion escogida es correcta

	//Se repite el proceso hasta que se genere una posicion correcta
	while(disponible==false){ //La posicion escogida no es correcta
		//Se establece la posicion del barco de manera aleatoria
		x=rand()%modulo;
		y=rand()%modulo;
		
		disponible=comprobarPosicion(x, y, b); //Se comprueba que la posicion escogida es correcta
	}

	b.grabarPosiciones(x, y); //Se coloca el barco en el tablero
}

//Establece los valores iniciales de las casillas del tablero
void Tablero:: inicializarTablero(){

	//Se generan los distintos barcos
	Barco b41(4);
	Barco b32(3), b31(3);
	Barco b23(2), b22(2), b21(2);
	Barco b14(1), b13(1), b12(1), b11(1);

	int x, y;

	//Se colocan todos los barcos en el tablero
	generarPosicion(x, y, b41);
	setBarco(b41, x, y);

	generarPosicion(x, y, b32);
	setBarco(b32, x, y);

	generarPosicion(x, y, b31);
	setBarco(b31, x, y);

	generarPosicion(x, y, b23);
	setBarco(b23, x, y);

	generarPosicion(x, y, b22);
	setBarco(b22, x, y);

	generarPosicion(x, y, b21);
	setBarco(b21, x, y);

	generarPosicion(x, y, b14);
	setBarco(b14, x, y);

	generarPosicion(x, y, b13);
	setBarco(b13, x, y);

	generarPosicion(x, y, b12);
	setBarco(b12, x, y);

	generarPosicion(x, y, b11);
	setBarco(b11, x, y);



}
