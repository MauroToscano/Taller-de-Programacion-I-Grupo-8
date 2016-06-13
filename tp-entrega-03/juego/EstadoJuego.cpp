#include "EstadoJuego.h"


EstadoJuego::EstadoJuego(EstadoAvion** estadoAvionJugador, int cantidadDeJugadores){
	for(int i = 0; i < cantidadDeJugadores; i++){
		estadoDeLosAviones.push_back(estadoAvionJugador[i]);
	}
	this->evento = new Evento(noHayNingunEventoEspecial);
}

EstadoJuego::EstadoJuego(Evento evento){
	this->evento = new Evento(evento.getNumeroDeEvento());
}

EstadoJuego::EstadoJuego(std::list<EstadoAvion*> estadoAvion){
	estadoDeLosAviones = estadoAvion;
	this->evento = new Evento(noHayNingunEventoEspecial);
} 

EstadoJuego::EstadoJuego(std::list<EstadoAvion*> estadoAvion, Evento evento){
	estadoDeLosAviones = estadoAvion;
	this->evento = new Evento(evento.getNumeroDeEvento());
} 

EstadoJuego::~EstadoJuego(){
	delete evento;
	std::list<EstadoAvion*> lista;
	std::list<EstadoAvion*>::iterator it;
	for (it = lista.begin(); it != lista.end(); it++) {
		delete (*it);
	}
	lista.clear();
}

std::list<EstadoAvion*> EstadoJuego::getEstadoDeLosAviones(){
	return estadoDeLosAviones;
}

Evento* EstadoJuego::obtenerEvento(){
	return evento;
}