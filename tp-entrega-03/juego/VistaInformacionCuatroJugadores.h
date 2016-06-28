#pragma once

#include <list>
#include "EstadoJugador.h"
#include "VistaInformacion.h"

using namespace std;

class VistaInformacionCuatroJugadores : public VistaInformacion {

public:
	VistaInformacionCuatroJugadores(SDL_Renderer* renderer);
	virtual ~VistaInformacionCuatroJugadores(void);
	void mostrar(std::list<EstadoJugador> estadosJugadores);
private:
	Etiqueta* usuario1;
	Etiqueta* usuario2;
	Etiqueta* usuario3;
	Etiqueta* usuario4;
	Etiqueta* puntos1;
	Etiqueta* puntos2;
	Etiqueta* puntos3;
	Etiqueta* puntos4;
	Etiqueta* usuarioGanadaor;
	Etiqueta* puntosGanador;
};
