#ifndef FUNCIONESSERVIDOR_HPP
#define FUNCIONESSERVIDOR_HPP
#include <string>
#include <vector>

#include "partida.hpp"
#include "tablero.hpp"
#include "usuario.hpp"

#define MSG_SIZE 250

bool leerUsuario(char buffer[MSG_SIZE],std::vector <Usuario> &usuarios,int descriptor);
bool leerPassword(char buffer[MSG_SIZE],std::vector <Usuario> &usuarios,int descriptor);
bool registro(char buffer[MSG_SIZE],int descriptor);
void salirCliente(int socket, fd_set * readfds,std::vector <Usuario> &usuarios, std::vector <Usuario> &cola_espera, std::vector <Partida> &partidas);
void deMatrizaVector(std::vector < std::vector <char> > matriz,std::string &vector);
void enviarTableros(Partida p);
std::vector <std::string> dividirCadena(std::string cadena,std::string separador);

void crearPartida(std::vector <Usuario> &usuarios, std::vector <Usuario> &cola_espera, std::vector <Partida> &partidas);

#endif
