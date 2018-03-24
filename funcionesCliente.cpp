#include "funcionesCliente.hpp"
#include "macros.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

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


void deVectoraMatriz(char matriz[10][10],char vector[]){
	int i;
	int j=0;
	int k=0;
	
	for(i=0;i<(int)strlen(vector);i++){
		if(i%2==0){
			matriz[j][k]=vector[i];
			
			if(k==9){
				k=0;
				
				if(j==9){
					break;
				}
				
				else{
					j++;
				}
			}
			
			else{
				k++;
			}
		}
	}
}

void imprimeMatriz(char matriz[][10]){
	//Se imprime el tablero
	std::cout << "     A   B   C   D   E   F   G   H   I   J" << std::endl;
	for(int i=0; i<10; i++){
		if(i<9)
			std::cout << i+1 << "  ";
		else
			std::cout << i+1 << " ";
		
		for (int j=0; j<10; j++){
			if(matriz[i][j]=='B')
				std::cout << "| "<< IGREEN << matriz[i][j] << RESET << " ";
			else if(matriz[i][j]=='A')
				std::cout << "| " << IBLUE << matriz[i][j] << RESET << " ";
			else
				std::cout << "| "<< matriz[i][j] << " ";
		
		}
		std::cout << "|" << std::endl;
	}

	std::cout << std::endl;
}


