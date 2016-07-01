#include "FakeAvionMini.h"

 FakeAvionMini::FakeAvionMini(int x,int y) : 
					FakeAvionEnemigo(x,y,anchoAvionMiddle,altoAvionMiddle,3){
	meMoviAbajoDeLaPantalla = false;
	id = idAvionMini;
	puntosDeVida = puntosDeVidaMaximoMiddle;
	puntosQueOtorgaAlSerImpactado = 0;
	puntosQueOtorgaAlSerDestruido = 50;
}