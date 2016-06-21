#include "Avion.h"

Avion::Avion(int ventanaAncho, int ventanaAlto, AvionView* avionView, BalaView* balaView) {
	//Las posicion inicial queda sin definir
	this->ventanaAncho = ventanaAncho;
	this->ventanaAlto = ventanaAlto;
	this->altoAvion = avionView->spriteXml->getAlto();
	this->anchoAvion = avionView->spriteXml->getAncho();
	this->velocidad = avionView->avionModel->velAvion;
    velocidadX = 0;
    velocidadY = 0;
	cantDeFotogramas = avionView->spriteXml->getCantidad();
	frame = 0;
	rollFlag = false;
	id = avionView->avionModel->id;
	this->balaView = balaView;
	centroProyectil = balaView->spriteXml->getAncho()/2;
	superficieQueOcupo = SuperficieOcupada(0,0,anchoAvion,altoAvion);;
}

Avion::~Avion() {
	std::list<Proyectil*>::iterator it;
	for (it = proyectiles.begin(); it != proyectiles.end(); it++) {
		delete (*it);
	}
}

void Avion::setPosicion(Posicion pos) {
	superficieQueOcupo.moverAPosicion(pos);
}

SuperficieOcupada Avion::actualizarPosicionEnX(){
	SuperficieOcupada hitbox;
	hitbox = superficieQueOcupo.desplazarEnXObteniendoHitbox(velocidadX);

	int miPosicionEnX;
	miPosicionEnX = superficieQueOcupo.obtenerPosicion().getPosX();

	if( miPosicionEnX< 0){
		superficieQueOcupo.moverAX(0);
	}

	if( miPosicionEnX + anchoAvion > this->ventanaAncho ){
		superficieQueOcupo.moverAX(this->ventanaAncho - anchoAvion);
	}
	return hitbox;
}

SuperficieOcupada Avion::actualizarPosicionEnY(){
	SuperficieOcupada hitbox;
	superficieQueOcupo.desplazarEnYObteniendoHitbox(velocidadY);
	int miPosicionEnY;
	miPosicionEnY = superficieQueOcupo.obtenerPosicion().getPosY();

	if( miPosicionEnY < 0){
		superficieQueOcupo.moverAY(0);
	}

	if( miPosicionEnY + altoAvion > this->ventanaAlto ){
		superficieQueOcupo.moverAY(this->ventanaAlto - altoAvion);
	}
	return hitbox;
}

void Avion::continuarElRoll(){
	if ((frame / cantDeFotogramas) >= cantDeFotogramas - 1){
			frame = 0;
			rollFlag = false;
	}
	frame++;
}

void Avion::continuarMovimientoDelAvion(){
	SuperficieOcupada hitbox;
	if(!rollFlag){
		hitbox = actualizarPosicionEnX();

		actualizarPosicionEnY();
	}else{
		continuarElRoll();
	}
}

void Avion::continuarMovimientoDelAvion(FakeAvionEnemigo avionEnemigo){
	//Los movimientos se hacen unidimensionalmente
	//Primero en X y luego en Y
	SuperficieOcupada hitbox;
	if(!rollFlag){
		hitbox = actualizarPosicionEnX();
		if(hitbox.meSolapoCon(avionEnemigo.obtenerSuperficieOcupada())){
			//Un comportamiento cuando colisiono con un avionEnemigo
			this->disparar();
		}
		hitbox = actualizarPosicionEnY();
		if(hitbox.meSolapoCon(avionEnemigo.obtenerSuperficieOcupada())){
			//Un comportamiento cuando colisiono con un avionEnemigo
			this->disparar();
		}
	//Si estoy haciendo un roll, no colisiono tampoco
	}else{
		continuarElRoll();
	}
}
void Avion::continuarMovimientoDeLosProyectiles(){
	std::list<Proyectil*>::iterator it;
	for (it = proyectiles.begin(); it != proyectiles.end(); it++) {
		if ((*it)->estaEnPantalla()) {
			(*it)->mover();
		}
	}
}

void Avion::eliminarLosProyectilesQueSalieronDeLaPantalla(){
		Proyectil* ultimoProyectil = proyectiles.front();
		if(!ultimoProyectil->estaEnPantalla() ){
			proyectiles.pop_front();
			delete ultimoProyectil;
		}
}
void Avion::mover() {
	continuarMovimientoDelAvion();
	//Avanzo los proyectiles
	continuarMovimientoDeLosProyectiles();
	//Si hay proyectiles
	if(!proyectiles.empty())
		eliminarLosProyectilesQueSalieronDeLaPantalla();

}

void Avion::mover(FakeAvionEnemigo avionEnemigo) {
	continuarMovimientoDelAvion(avionEnemigo);
	//Avanzo los proyectiles
	continuarMovimientoDeLosProyectiles();
	//Si hay proyectiles
	if(!proyectiles.empty())
		eliminarLosProyectilesQueSalieronDeLaPantalla();

}

EstadoAvion* Avion::getEstado() {
	int miPosicionEnY;
	miPosicionEnY = superficieQueOcupo.obtenerPosicion().getPosY();
	int miPosicionEnX;
	miPosicionEnX = superficieQueOcupo.obtenerPosicion().getPosX();
	EstadoAvion*  estado =  new EstadoAvion(id, frame, miPosicionEnX, miPosicionEnY);
	std::list<EstadoProyectil*> lista;
	std::list<Proyectil*>::iterator it;
	for (it = proyectiles.begin(); it != proyectiles.end(); it++) {
		estado->agregarEstadoProyectil((*it)->createEstado());
	}
	return estado;
}

std::list<EstadoProyectil*> Avion::getEstadoProyectiles() {
	std::list<EstadoProyectil*> lista;
	std::list<Proyectil*>::iterator it;
	for (it = proyectiles.begin(); it != proyectiles.end(); it++) {
		lista.push_back((*it)->createEstado());
	}
	return lista;
}

void Avion::realizarAccionEnBaseA(Evento* evento){
	switch(evento->getNumeroDeEvento()){
    case apretadaLaTeclaDeMovimientoHaciaIzquierda  :
		this->darVelocidadHaciaLaIzquierda();
       break;
    case soltadaLaTeclaDeMovimientoHaciaIzquierda   :
		this->darVelocidadHaciaLaDerecha();
       break;
	case apretadaLaTeclaDeMovimientoHaciaDerecha   :
		this->darVelocidadHaciaLaDerecha();
       break;
	case soltadaLaTeclaDeMovimientoHaciaDerecha    :
		this->darVelocidadHaciaLaIzquierda();
       break;
	case apretadaLaTeclaDeMovimientoHaciaArriba   :
		this->darVelocidadHaciaArriba();
       break;
	case soltadaLaTeclaDeMovimientoHaciaArriba  :
		this->darVelocidadHaciaAbajo();
       break;
	case apretadaLaTeclaDeMovimientoHaciaAbajo  :
		this->darVelocidadHaciaAbajo();
       break;
	case soltadaLaTeclaDeMovimientoHaciaAbajo  :
		this->darVelocidadHaciaArriba();
       break;
	case apretadaLaTeclaDeDisparo :
		this->disparar();
       break;
	case apretadaLaTeclaDeRoll  :
		this->hacerUnRoll();
       break;
	}
}
//BUG-000 aca hay un error pues plantea que se suma la misma velocidad en ambas coordenadas
//deltaY <-- (+vel)
void Avion::darVelocidadHaciaArriba(){
	velocidadY += velocidad;
}
//deltaY <-- (-vel)
void Avion::darVelocidadHaciaAbajo(){
	velocidadY -= velocidad;
}
//deltaX <-- (+vel)
void Avion::darVelocidadHaciaLaDerecha(){
	velocidadX += velocidad;
}
//deltaX <-- (-vel)
void Avion::darVelocidadHaciaLaIzquierda(){
	velocidadX -= velocidad;
}

void Avion::disparar(){
	int miPosicionEnY;
	miPosicionEnY = superficieQueOcupo.obtenerPosicion().getPosY();
	int miPosicionEnX;
	miPosicionEnX = superficieQueOcupo.obtenerPosicion().getPosX();

	if(!rollFlag){
		Proyectil* proyectil = new Proyectil(this->balaView);
		proyectil->setCoordenasDeComienzo(miPosicionEnX + (anchoAvion / 2) - centroProyectil, this->ventanaAlto - (miPosicionEnY + altoAvion));
		proyectiles.push_back(proyectil);
	}
}

void Avion::hacerUnRoll(){
	rollFlag = true;
}