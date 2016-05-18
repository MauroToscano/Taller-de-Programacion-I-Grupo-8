#pragma once
#ifndef _MAPA_H_
#define _MAPA_H_
#include "Textura.h"
#include "ElementoDelMapa.h"
#include "ConfiguracionJuegoXML.h"
#include "../common/view/ElementoView.h"
#include <list>
using namespace std;

class Mapa
{
public:
	static Mapa* getInstace();

	void inicializar(SDL_Renderer* renderer);
	void reiniciar();
	~Mapa();
	void dibujarFondoYElementos();
	//Las posiciones del mapa empiezan en la 1,1 que es la esquina inferior izquierda.
	void crearElemento(ElementoView * elementoView);

private:

	static bool instanceFlag;
	static Mapa* instance;

	Mapa();

	Textura* texturaMapa;
	int anchoMapa;
	int altoMapa;
	int pixelesAvanzados;
	int scrollingOffset;
	int tamanioMaximoMapa;
	SDL_Renderer* renderer;
	int cantidadDePixelesQuePasaron;
	std::list<ElementoDelMapa*> elementosDelMapa;
	void dibujarElementos();
};

#endif //_MAPA_H_