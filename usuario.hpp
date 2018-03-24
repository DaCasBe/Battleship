#ifndef USUARIO_HPP
#define USUARIO_HPP

#include <string>

//Estados del usuario
#define CONECTADO -1
#define NOMBRE 0
#define LOGEADO 1
#define EN_COLA 2
#define EN_PARTIDA 3

class Usuario{
	private:
		int _sd; //Socket del usuario
		std::string _usuario; //Nombre del usuario
		std::string _password; //Password del usuario
		int _estado; //Estado del usuario
		
	public:
		//Constructor del usuario
		Usuario(){
			setEstado(CONECTADO); //El usuario estara conectado por defecto
		}
		
		//Devuelve el socket del usuario
		int getSd(){
			return _sd;
		}

		//Modifica el socket del usuario
		void setSd(int sd){
			_sd=sd;
		}

		//Devuelve el nombre del usuario
		std::string getUsuario(){
			return _usuario;
		}

		//Modifica el nombre del usuario
		void setUsuario(std::string usuario){
			_usuario=usuario;
		}

		//Devuelve la password del usuario
		std::string getPassword(){
			return _password;
		}

		//Modifica la password del usuario
		void setPassword(std::string password){
			_password=password;
		}

		//Devuelve el estado del usuario
		int getEstado(){
			return _estado;
		}

		//Modifica el estado del usuario
		void setEstado(int estado){
			_estado=estado;
		}
};

#endif
