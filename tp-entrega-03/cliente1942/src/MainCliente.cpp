#include "MainCliente.h"

bool MainCliente::instanceFlag = false;
MainCliente* MainCliente::single = NULL;

bool MainCliente::serverDesconectado = true;
bool MainCliente::cerrarConexion = true;

MainCliente* MainCliente::getInstance(){

	if(! instanceFlag){
		single = new MainCliente();
		instanceFlag = true;
		return single;
	}
	else{
		return single;
	}
}

MainCliente::MainCliente(){
	this->conectado = false;
	// aca deberia de obtener el ip, port y cargar los mensajes en el map
	this->ip = "";
	this->port = -1;
	this->parserx = new ParserXml();
	this->servidorXml = new ServidorXml();
}

MainCliente::~MainCliente(){
	delete this->parserx;
	if (this->servidorXml != NULL)
		delete this->servidorXml;
}

// Ahora se carga la IP y el Puerto por consola, se deja esta funci�n para configuraciones del cliente
void MainCliente::parsearArchivoXml(int argc, char* argv[]){
	getParserXml()->cargarXmlCliente(argc,argv);
	int res = getParserXml()->validarXmlArchivoCliente();
	if (res < 0){
		printf("\nERROR: Error semantico\n");
		Log::getInstance()->error("ERROR: Error semantico.");
		getParserXml()->cargarXmlCliente(0,argv);
	}
	//luego de la carga crea los datos a partir del XML
	ClienteXml * clienteXml = getParserXml()->createDataClienteXml();
	//se cargan los datos desde el cliente
	//copia la ip
	this->ip.assign(clienteXml->getConexionXmlCopy()->getIp());
	//copia el puerto
	int puerto = clienteXml->getConexionXmlCopy()->getPuerto();
	char cadena[10];
	sprintf(cadena,"%d",puerto);
	this->port.assign(cadena);
	// luego de usarlo se debe borrar
	delete clienteXml;
}

ParserXml * MainCliente::getParserXml(){
	return this->parserx;
}

int MainCliente::chequearConexion(int len){
	if (len == 0){
		printf("\n No llego el mensaje, se desconecto el servidor\n");
		conectado=false;
		return -1;
	}
	else if (len < 0){
		conectado=false;
		int error = WSAGetLastError();

		if(error == WSAENOTCONN || error == WSAECONNRESET)
			printf("\n Se a desconectado inesperadamente el servidor\n");
		else if (error == WSAENETDOWN)
			printf("\nRed caida\n");
		else
			printf("\nError en conexion con el servidor\n");
		return -1;
	}

	return 0;
}

int MainCliente::inicializarConexion(){

	//Inicializamos
	WSAStartup(MAKEWORD(2,2),&wsa);

	//resolvemos el nombre de dominio localhost, esto se resolver� a 127.0.0.1
	host=gethostbyname(ip.c_str());

	//creamos el socket
	sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (sock==-1)
	{
		Log::getInstance()->error(" al crear el socket.");
		printf("Error al crear el socket");
		return -1;
	}
	int c;
	setsockopt (sock, IPPROTO_TCP, SO_REUSEADDR | SOCK_STREAM, (char*)&c, sizeof(int));
	//Definimos la direcci�n a conectar que hemos recibido desde el gethostbyname
	//y decimos que el puerto al que deber� conectar es el 9999 con el protocolo ipv4
	direc.sin_family=AF_INET;
	direc.sin_port=htons(atoi(port.c_str()));
	direc.sin_addr = *((struct in_addr *)host->h_addr);
	memset(direc.sin_zero,0,8);

	return 0;
}
void MainCliente::grabarEnElLogLaDesconexion(int len){
	if (len == 0){
		Log::getInstance()->info( "Se ha desconectado el server");
	}
	else if (len < 0){
		// Si es -1 hay un error en la conexion
		int error = WSAGetLastError();

		if(error == WSAENOTCONN || error == WSAECONNRESET){
			Log::getInstance()->error( "Se ha desconectado inesperadamente el server");
		}
		else if (error == WSAENETDOWN)
			Log::getInstance()->error( "Red caida");
		else
			Log::getInstance()->error( "Error de conexion");
	}
}

int MainCliente::fun_recibirMensajes(void* ptrSock) {
	return MainCliente::getInstance()->recibirMensajes(ptrSock);
}

int MainCliente::recibirMensajes(void* ptrSock){
	bool primerMensaje =true;
	char bufferEntrada[MAX_BUFFER];
	while (!cerrarConexion && !serverDesconectado){
		int len=MensajeSeguro::recibir(*((SOCKET*)ptrSock),bufferEntrada); //recibimos los datos que envie
		if (len>0){
			// espero el mensaje del server, comenzar el juego para desbloquear SDL
			EstadoJuego* stJuego = NULL;
			if(primerMensaje){
				// en realidad no necesito chequear el mensaje
				int offset = Protocolo::decodificar(bufferEntrada,stJuego);
				if (stJuego->obtenerEvento()->getNumeroDeEvento() == comienzaLaPartida){
					// Si el server nos envia respuesta que debemos iniciar el juego
					Log::getInstance()->info("Comenzar el juego");
					cout<<"Comenzar el juego"<<endl;
					VistaJuego::getInstance()->setJugar();
					primerMensaje=false;
					delete stJuego;
				}
			} else {
				//si seguimos conectados
				int offset = Protocolo::decodificar(bufferEntrada,stJuego);
				VistaJuego::getInstance()->actualizarEstadoJuego(stJuego);
				//Un mensaje con id -2 indica que se reinicio el mapa
				/*
				if(stAvion->getId() == -2){
					//se debe recrear el servidor pues el anterior ya no sirve
					recreateServidorXml();
					Protocolo::decodificar(bufferEntrada + offset, this->servidorXml);
					VistaJuego::getInstance()->reiniciar(this->servidorXml, 0);
				}*/
			}
		}
		else{
			grabarEnElLogLaDesconexion(len);
			serverDesconectado = true;
		}
	}
	return 0;
}

void MainCliente::cargarIP() {
	system("CLS");
	printf("\n%s\n", "------------------------------------------------------------------------");
	printf("Ingrese IP\n");
	string ip = "";
	while (strcmp(ip.c_str(), "") == 0) {
		cin>>ip;
	}
	this->ip = ip;
}

void MainCliente::cargarPuerto() {
	system("CLS");
	printf("\n%s\n", "------------------------------------------------------------------------");
	printf("Ingrese Puerto\n");
	string puerto = "";
	while (strcmp(puerto.c_str(), "") == 0) {
		cin>>puerto;
	}
	this->port = puerto;
}

void MainCliente::cargarIpYPuerto() {
	cargarIP();
	cargarPuerto();
}

void MainCliente::cargarNombreDeUsuario(Jugador * jugador) {
	system("CLS");
	printf("\n%s\n", "------------------------------------------------------------------------");
	printf("Ingrese su nombre de usuario\n");
	string nombreDeUsuario = "";
	while (strcmp(nombreDeUsuario.c_str(), "") == 0) {
		cin>>nombreDeUsuario;
	}
	jugador->nombreDeUsuario.assign(nombreDeUsuario);
}

int MainCliente::conectar(){

#ifndef FAKE_DEBUG_CLIENTE
	cargarIpYPuerto();
	cargarNombreDeUsuario(VistaJuego::getInstance()->getJugador());
#else
	VistaJuego::getInstance()->getJugador()->nombreDeUsuario.assign("Cliente-A");
#endif

	inicializarConexion();

	if(conectado == true){
		Log::getInstance()->warn(" el cliente ya se encuentra conectado.");
		printf("ya se encuentra conectado \n");
	}
	else{
		//Intentamos establecer la conexi�n
		int conex=connect(sock,(sockaddr *)&direc, sizeof(sockaddr));
		if (conex==-1) //si no se ha podido conectar porque no se ha encontrado el host o no est� el puerto abierto
		{
			Log::getInstance()->error(" no se ha podido conectar.");
			printf("No se ha podido conectar\n");
			printf("%i", conex);
			return -1;
		}
		else{
			// Se envia un mensaje al servidor para que valide el nombre de usuario
			MensajeXml mensajeUsuario;
			mensajeUsuario.setValor((char*)VistaJuego::getInstance()->getJugador()->nombreDeUsuario.c_str(), strlen(VistaJuego::getInstance()->getJugador()->nombreDeUsuario.c_str()));
			mensajeUsuario.setTipo(TIPO_STRING);
			mensajeUsuario.calculateSizeBytes();

			char bufferSalida [MAX_BUFFER];
			int size = Protocolo::codificar(mensajeUsuario, bufferSalida);
			MensajeSeguro::enviar(sock, bufferSalida, size);
			char bufferEntrada[MAX_BUFFER];

			// Se rerrcibe la confirmaci�n de la validaci�n del nombre de usuario
			int len2 = MensajeSeguro::recibir(sock,bufferEntrada);

			if (len2 <= 0){// Es un error
				chequearConexion(len2);
			}
			else{
				//decodificar el mensaje
				MensajeXml mensaXml;
				int offset = Protocolo::decodificar(bufferEntrada,&mensaXml);
				char * respuesta = mensaXml.getValor();

				if (strcmp(respuesta,MSJ_CONEX_ACEPT) == 0){

					// Si el server nos envia respuesta es que la conexion ha sido satisfactoria
					Log::getInstance()->info("El cliente se ha conectado correctamente.");
					conectado = true;
					MainCliente::serverDesconectado = false;
					MainCliente::cerrarConexion = false;

					offset += Protocolo::decodificar(bufferEntrada + offset,&mensaXml);
					char* idUsuario = mensaXml.getValor();

					Posicion posicion;
					// Se decodifica la posicion inicial desde donde arranca el avi�n
					offset += Protocolo::decodificar(bufferEntrada + offset, &posicion);

					Posicion posicionMapa;
					// Se decodifica la posicion del mapa, que tiene solo una componente: Y
					offset += Protocolo::decodificar(bufferEntrada + offset, &posicionMapa);

					//se procede a decodificar el resto del mensaje
					//se decodifica el escenario completo
					offset += Protocolo::decodificar(bufferEntrada + offset,this->servidorXml);

					// Creo un hilo para escuchar los mensajes
					receptor=SDL_CreateThread(fun_recibirMensajes, "recibirMensajes", &sock);
					
					VistaJuego::getInstance()->getJugador()->setIdCliente(atoi(idUsuario));
					VistaJuego::getInstance()->getJugador()->setPosicionAvion(posicion);

					VistaJuego::getInstance()->readServidorXml(this->servidorXml);
					VistaJuego::getInstance()->agregarObservadorAlControlador(this);
					VistaJuego::getInstance()->inicializar();
					VistaJuego::getInstance()->ejecutar(this->servidorXml, posicionMapa.getPosY());
					// se termina el programa cuando el usuario hace click en x del SDL_window
					terminarElCliente();
				}
				else if (strcmp(respuesta,MSJ_SUPERO_MAX) == 0){
					// El server envia un mensaje al superar la cantidad de clientes

					Log::getInstance()->error(bufferEntrada);
					printf("Respuesta servidor:> %s\n",MSJ_SUPERO_MAX);

					shutdown(sock,2);
					closesocket(sock);
				}
				else if (strcmp(respuesta, MSJ_USR_YA_CONECT) == 0) {
					// El server ya tiene un usuario igual y est� conectado

					Log::getInstance()->error(bufferEntrada);
					printf("Respuesta servidor:> %s\n",MSJ_USR_YA_CONECT);
					desconectar();
				}
			}
		}
	}
	return 0;
}
int MainCliente::terminarElCliente(){

	SDL_Delay(1000);
	this->salir();
	this->opt=OPT_SALIR;
	return 0;
}
int MainCliente::desconectar(){
	shutdown(sock,2);
	closesocket(sock);
	conectado=false;
	MainCliente::cerrarConexion = false;
	return 0;
}

int MainCliente::salir(){
	mapMensajes.clear(); // chequear si se liberan los string
	// liberar la memoria de los mensajes
	desconectar();
	return 0;
}

/**
 * se encarga de liberar la memoria del servidorXML
 * y volver a alocar memoria para el nuevo servidorXML
 */
void MainCliente::recreateServidorXml(){
	if (this->servidorXml != NULL){
		delete this->servidorXml;
		this->servidorXml = new ServidorXml();
	}
}

/**
* muestra el menu y direcciona a las opciones
*/
int MainCliente::menu(){
	opt = 0;
	while (opt != OPT_SALIR){
		printf("\n%s\n", "------------------------------------------------------------------------");
		if(conectado)
			std::cout<<"Se encuentra: CONECTADO" <<std::endl;
		else
			std::cout<<"Se encuentra: DESCONECTADO" <<std::endl;
		printf("\n<1> CONECTAR");
		printf("\n<3> SALIR");
		printf("\n");
		string numstring;
		cin>>numstring;
		opt=atoi(numstring.c_str());
		switch (opt){
		case OPT_CONECTAR:
			conectar();
			break;
		case OPT_SALIR:
			salir();
			break;
		}
	}
	return 0;
}

int MainCliente::actualizar(Evento* evento){

	char * buffEnvio = new char[MAX_BUFFER];
	int sizeBytesTotalLista = Protocolo::codificar(*evento,buffEnvio);
	int estado=0;
	if((estado=chequearConexion(MensajeSeguro::enviar(sock,buffEnvio,sizeBytesTotalLista)))<0) {
		printf("No se pudo enviar el movimiento, el cliente termina \n");
		system("PAUSE");
	}
	delete[] buffEnvio;
	delete evento;
	return estado;
}