#include "ConfiguracionJuegoXML.h"

bool ConfiguracionJuegoXML::instanceFlag = false;
ConfiguracionJuegoXML* ConfiguracionJuegoXML::instance = NULL;

ConfiguracionJuegoXML::ConfiguracionJuegoXML(void) {
}

ConfiguracionJuegoXML::~ConfiguracionJuegoXML(void) {
}

ConfiguracionJuegoXML* ConfiguracionJuegoXML::getInstance() {
	
	if(!instanceFlag){
        instance = new ConfiguracionJuegoXML();
        instanceFlag = true;
    }
    return instance;
}

void ConfiguracionJuegoXML::setCaracteristicasMapa(std::string fondo, std::string isla, std::string carrier,
																	int tamanioMaximoMapa) {
	pathImagenCarrier = carrier;
	pathImagenFondo = fondo;
	pathImagenIsla = isla;
	tamanioMaximoFondo = tamanioMaximoMapa;
}

void ConfiguracionJuegoXML::setCaracteristicasAvion(int id, std::string avion, int cantFotogramas, int anchoFotograma, int altoFotograma, int velocidad) {

	configAvion = new ConfiguracionAvionXML(id, avion, cantFotogramas, anchoFotograma, altoFotograma, velocidad);
}

void ConfiguracionJuegoXML::setCaracteristicasProyectil(std::string proyectil, int cantFotogramas, int anchoFotograma, int altoFotograma, int velocidad) {

	configProyectil = new ConfiguracionProyectilXML(proyectil, cantFotogramas, anchoFotograma, altoFotograma, velocidad);
}

ConfiguracionAvionXML* ConfiguracionJuegoXML::getConfiguracionAvion() {
	return configAvion;
}

ConfiguracionProyectilXML* ConfiguracionJuegoXML::getConfiguracionProyectil() {
	return configProyectil;
}

std::string ConfiguracionJuegoXML::getPathFondo() {
	return pathImagenFondo;
}


std::string ConfiguracionJuegoXML::getPathCarrier() {
	return pathImagenCarrier;
}

std::string ConfiguracionJuegoXML::getPathIsla() {
	return pathImagenIsla;
}

int ConfiguracionJuegoXML::getTamanioMaximoFondo() {
	return tamanioMaximoFondo;
}