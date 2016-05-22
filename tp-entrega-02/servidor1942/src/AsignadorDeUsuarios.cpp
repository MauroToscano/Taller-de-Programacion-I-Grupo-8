#include "AsignadorDeUsuarios.h"

AsignadorDeUsuarios::AsignadorDeUsuarios(int usuariosMaximos){
	cantidadMaximaDeUsuarios = usuariosMaximos;
	cantidadDeUsuariosActuales = 0;
	usuario = new Usuario[usuariosMaximos];
	for(int i = 0; i<usuariosMaximos; i++){
		usuario[i].estaConectado = false;
		usuario[i].estaAsignado = false;
		usuario[i].colaDeMensajesDeUsuario = new std::queue<EstadoAvionXml*>;
	}
}


AsignadorDeUsuarios::~AsignadorDeUsuarios(void)
{

}


bool AsignadorDeUsuarios::puedoTenerMasUsuarios(void)
{
	return (cantidadDeUsuariosActuales < cantidadMaximaDeUsuarios);
}

bool AsignadorDeUsuarios::nombreDeUsuarioExistente(string nombreDeUsuarioRecibido){
	bool encontreElNombre;
	encontreElNombre = false;
	for(int i = 0; i< cantidadMaximaDeUsuarios; i++){
		if(usuario[i].estaAsignado && !encontreElNombre){
			encontreElNombre = (usuario[i].nombreDeUsuario.compare(nombreDeUsuarioRecibido) == 0);
		}
	}

	return encontreElNombre;
}

bool AsignadorDeUsuarios::estaConectado(string nombre){

	std::list<Usuario*>::iterator it;
	int idUsuario;
	idUsuario = idUsuarioLlamado(nombre);
	if(idUsuario != -1)
		return  usuario[idUsuario].estaConectado;
	//Si el usuario no existe devuelve falso
	return false;
}
int AsignadorDeUsuarios::idUsuarioLlamado(string nombre){
	for(int i = 0; i< cantidadMaximaDeUsuarios; i++){
		if(usuario[i].nombreDeUsuario.compare(nombre) == 0){
			return(i);
		}
	}
	return -1;
}
int AsignadorDeUsuarios::reconectar(string nombreUsuario){

	std::list<Usuario*>::iterator it;
	int id;
	id = idUsuarioLlamado(nombreUsuario);
	if(id != -1)
		usuario[id].estaConectado = true;
	return id;
}
int AsignadorDeUsuarios::obtenerUnaIdLibre(){
	int i = -1;
	bool encontreUnaIdLibre = false;
	while(!encontreUnaIdLibre && i < cantidadMaximaDeUsuarios){
		i++;
		encontreUnaIdLibre = !usuario[i].estaAsignado;
	}
	return i;
}

int AsignadorDeUsuarios::crearUsuarioYObtenerId(string nombre){
	
	int idNuevoUsuario = -1;
	
	if (puedoTenerMasUsuarios()){

		idNuevoUsuario = obtenerUnaIdLibre();
		usuario[idNuevoUsuario].estaAsignado = true;
		usuario[idNuevoUsuario].estaConectado = true;
		usuario[idNuevoUsuario].nombreDeUsuario = nombre;
		cantidadDeUsuariosActuales++;
	}

	return idNuevoUsuario;
}

void AsignadorDeUsuarios::eliminarUsuario(int idUsuario){
	while(!usuario[idUsuario].colaDeMensajesDeUsuario->empty()){
		usuario[idUsuario].colaDeMensajesDeUsuario->pop();
	}
	usuario[idUsuario].estaAsignado = false;
	usuario[idUsuario].estaConectado = false;
	cantidadDeUsuariosActuales--;
}

void AsignadorDeUsuarios::desconectarUsuario(int idUsuario){
	int id;
	while(!usuario[idUsuario].colaDeMensajesDeUsuario->empty()){
		usuario[idUsuario].colaDeMensajesDeUsuario->pop();
	}
	usuario[idUsuario].estaConectado = false;
	cantidadDeUsuariosActuales--;
}
std::queue<EstadoAvionXml*>* AsignadorDeUsuarios::obtenerColaDeUsuario(int idUsuario){
	return usuario[idUsuario].colaDeMensajesDeUsuario;
}

int AsignadorDeUsuarios::cantidadDeUsuarios(){
	return cantidadDeUsuariosActuales;
}