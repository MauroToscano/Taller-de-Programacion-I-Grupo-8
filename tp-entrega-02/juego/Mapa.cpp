#include "Mapa.h"

bool Mapa::instanceFlag = false;
Mapa* Mapa::instance = NULL;

Mapa* Mapa::getInstace() {

	if(!instanceFlag){
        instance = new Mapa();
        instanceFlag = true;
    }
    return instance;
}

void Mapa::inicializar(SDL_Renderer* rendererRecibido) {

	ConfiguracionJuegoXML* configJuego = ConfiguracionJuegoXML::getInstance();

	tamanioMaximoMapa = configJuego->getTamanioMaximoFondo();
	renderer = rendererRecibido;

	texturaMapa= new Textura();
	if( !texturaMapa->cargarDeArchivo( configJuego->getPathFondo(),renderer ) ){
		texturaMapa->cargarDeArchivo( "fondoDefault.bmp",renderer );
	}
	/*
	texturaIsla= new Textura();
	if( !texturaIsla->cargarDeArchivo( configJuego->getPathIsla(),renderer) ){
		texturaIsla->cargarDeArchivo( "signoInterrogacion.bmp",renderer );
	}
	*/
	anchoMapa = texturaMapa->getWidth();
	altoMapa = texturaMapa->getHeight();	
}

Mapa::Mapa(){
	
	pixelesAvanzados = 0;
	scrollingOffset = 0;
	cantidadDePixelesQuePasaron = 0;
}


Mapa::~Mapa(){

	std::list<ElementoDelMapa*>::iterator it;

	for(it=elementosDelMapa.begin(); it!=elementosDelMapa.end(); it++)
		 delete (*it);
//	delete texturaIsla;
	delete texturaMapa;
}

void Mapa::dibujarElementos(){

	std::list<ElementoDelMapa*>::iterator it;
	
	for(it=elementosDelMapa.begin(); it!=elementosDelMapa.end(); it++)
	     (*it)->graficarseSiEstaEnPantalla(altoMapa + cantidadDePixelesQuePasaron, cantidadDePixelesQuePasaron);
}

void Mapa::dibujarFondoYElementos(){

	if(cantidadDePixelesQuePasaron > tamanioMaximoMapa)
		this->reiniciar();

	//Despu�s de la ultima posicion de la imagen de fondo sigue la primera
	if(scrollingOffset >= altoMapa)
		scrollingOffset = 0;
	texturaMapa->render(0, scrollingOffset, renderer);
	texturaMapa->render(0, scrollingOffset - altoMapa, renderer);
	dibujarElementos();
	scrollingOffset++;
	cantidadDePixelesQuePasaron++;
}
void Mapa::reiniciar(){

	cantidadDePixelesQuePasaron = 0;
	scrollingOffset = 0;
	std::list<ElementoDelMapa*>::iterator it;
	for(it=elementosDelMapa.begin(); it!=elementosDelMapa.end(); it++)
		(*it)->reiniciar();
}

void Mapa::crearElemento(ElementoView * elementoView){
//se crea el elemento a partir de los datos originales
	Textura * texturaElemento = new Textura();
	char * path = elementoView->spriteXml->getPath();
	texturaElemento->cargarDeArchivo(path,renderer );
	ElementoDelMapa* elementoDelMapa;
	elementoDelMapa = new ElementoDelMapa(elementoView->posicion.x,elementoView->posicion.y, renderer,texturaElemento);
	elementosDelMapa.push_back(elementoDelMapa);
}
