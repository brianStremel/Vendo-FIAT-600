//#include "suse.h"

////VARIABLES CONFIGURACION
//int listen_port;
//int metrics_timer;
//int max_multiprog;
//char** sem_ids;
//char** sem_init_value;
//char** sem_max;
//double alpha_sjf;
//
//
//t_log* log;
//int programasEnMemoria = 0;
//int cantidadSemaforos;
//t_list* listaSemaforos;
//t_list* listaNuevos;
//t_list* listaDeBloqueados;
//t_list* LISTAENEJECUCION;
//t_list* listaDeProgramas;
//
//
////AUXILIARES PARA BUSQUEDA
//int auxiliarParaId;
//int auxiliarParaIdPadre;
//char* auxiliarString;
//t_list* auxiliarListaParaBusqueda;
//double auxiliarParaBusquedaMenor;

//void load_suse_config() {
//	t_config* config = config_create("suse.config");
//	listen_port = config_get_int_value(config, "LISTEN_PORT");
//	metrics_timer = config_get_int_value(config, "METRICS_TIMER");
//	max_multiprog = config_get_int_value(config, "MAX_MULTIPROG");
//	sem_ids = config_get_array_value(config, "SEM_IDS");
//	sem_init_value = config_get_array_value(config, "SEM_INIT");
//	sem_max = config_get_array_value(config, "SEM_MAX");
//	alpha_sjf = config_get_double_value(config, "ALPHA_SJF");
//
//	config_destroy(config);
//}
//
//void cargarSemaforos() {
//	listaSemaforos = list_create();
//	cantidadSemaforos = strlen(sem_ids)/sizeof(char*);
//	t_semaforo* semaforoAuxiliar;
//
//	for(int i=0; i<cantidadSemaforos; i++) {
//		semaforoAuxiliar = malloc(sizeof(t_semaforo));
//		semaforoAuxiliar->nombre = malloc(strlen(sem_ids[i])+1);
//		strcpy(semaforoAuxiliar->nombre, sem_ids[i]);
//		semaforoAuxiliar->valor = atoi(sem_init_value[i]);
//		semaforoAuxiliar->valorMaximo = atoi(sem_max[i]);
//		semaforoAuxiliar->colaBloqueo = queue_create();
//		list_add(listaSemaforos, semaforoAuxiliar);
//	}
//}

//bool esHiloPorId(void* unHilo) {
//	t_hilo* casteo = (t_hilo*) unHilo;
//	return (casteo->id == auxiliarParaId) && (casteo->idPadre->id == auxiliarParaIdPadre);
//}
//bool esProgramaPorId(void* unPrograma) {
//	t_programa* casteo = (t_programa*) unPrograma;
//	return (casteo->id == auxiliarParaId);
//}

//void suseCreate(int threadId, t_programa* padreId) {
//	t_hilo* nuevo = malloc(sizeof(t_hilo));
//	nuevo->id = threadId;
//	nuevo->idPadre = padreId;
//	nuevo->estado = NEW;
//	nuevo->tiempoInicial = 0;
//	nuevo->tiempoEspera = 0;
//	nuevo->tiempoCpu = 0;
//	nuevo->estimadoSJF = 0;
//	nuevo->bloqueadoPor = list_create();
//
//	list_add(padreId->listaDeHilos, nuevo);
//	list_add(listaNuevos, nuevo);
//	list_add(LISTAENEJECUCION, nuevo);
//
//}

//bool esElMenor(void* hilo) {
//	t_hilo* unHilo = (t_hilo*)hilo;
//	return auxiliarParaBusquedaMenor <= unHilo->estimadoSJF;
//}
//
//bool tieneMenorEstimado(void* hilo) {
//	t_hilo* unHilo = (t_hilo*)hilo;
//	auxiliarParaBusquedaMenor = unHilo->estimadoSJF;
//	return list_all_satisfy(auxiliarListaParaBusqueda, esElMenor);
//}

//void suseScheduleNext(t_programa* programa) {
//	if(list_size(programa->listaDeReady) > 0) {
//		auxiliarListaParaBusqueda = programa->listaDeReady;
//		t_hilo* hilo = list_remove_by_condition(programa->listaDeReady, tieneMenorEstimado);
//
//		if(programa->enEjecucion != NULL) {
//			programa->enEjecucion->estado = READY;
//			list_add(programa->listaDeReady, programa->enEjecucion);
//			programa->enEjecucion = NULL;
//		}
//
//		hilo->estado = EXEC;
//		programa->enEjecucion = hilo;
//
//		send_message(programa->id, SUSE_SCHEDULE_NEXT, &hilo->id, sizeof(int));
//	}
//	else
//		send_message(programa->id, ERROR_MESSAGE, NULL, 0);
//}

//void estimarDuracionHilo(t_hilo* unHilo, double duracion) {
//	unHilo->estimadoSJF = duracion * alpha_sjf + (1-alpha_sjf) * (unHilo->estimadoSJF); //ALGORITMO SJF
//	printf("Se ejecuto SJF\n");
//}

//void cargarHilosAReady() {
//	while(programasEnMemoria < max_multiprog && list_size(listaNuevos) != 0) {
//		t_hilo* hilo = list_remove(listaNuevos, 0);
//		hilo->estado = READY;
//		list_add(hilo->idPadre->listaDeReady, hilo);
//		programasEnMemoria ++;
//	}
//}

//bool buscarSemaforoPorNombre(t_semaforo unSemaforo) {
//	if(strcmp(unSemaforo.nombre, auxiliarString) == 0) {
//		return true;
//	}
//	else {
//		return false;
//	}
//}

void suseWait(int threadId, char* nombreSemaforo, t_programa* padre) {
	t_hilo* unHilo;

	//Se busca el semaforo
	auxiliarString = malloc(strlen(nombreSemaforo)+1);
	strcpy(auxiliarString, nombreSemaforo);
	t_semaforo* unSemaforo = list_find(listaSemaforos, buscarSemaforoPorNombre);
	free(auxiliarString);

	//Se le resta en uno su valor
	unSemaforo->valor --;


	//Si queda negativo se agrega el thread a la lista de bloqueados
	//Y se le agrega al thread por quien fue bloqueado
	if(unSemaforo->valor < 0) {
		queue_push(unSemaforo->colaBloqueo, threadId);

		auxiliarParaId = threadId;
		auxiliarParaIdPadre = padre->id;
		unHilo = list_find(padre->listaDeHilos, esHiloPorId);
		list_add(unHilo->bloqueadoPor, unSemaforo);

		if(unHilo->estado != BLOCKED) {

			unHilo->estado = BLOCKED;
			if(padre->enEjecucion->id == threadId) {
				list_add(listaDeBloqueados, unHilo);
				padre->enEjecucion = NULL;
			}
			else if(list_any_satisfy(padre->listaDeReady, esHiloPorId)) {
				list_remove_by_condition(padre->listaDeReady, esHiloPorId);

			}
			else if(list_any_satisfy(listaNuevos, esHiloPorId)) {
				list_remove_by_condition(listaNuevos, esHiloPorId);
			}

		}
	}
}

void suseSignal(int threadId, char* semaforo, t_programa* padre) {

}

void suseClose(int id, t_programa* programa) {
	auxiliarParaId = id;
	auxiliarParaIdPadre = programa->id;
	t_hilo* auxiliar;

	if(list_any_satisfy(LISTAENEJECUCION, esHiloPorId)) {
		if(programa->enEjecucion->id == id) {
			programa->enEjecucion->estado = EXIT;
			list_add(listaExit, programa->enEjecucion);
			programa->enEjecucion = NULL;
		}
		else if(list_any_satisfy(programa->listaDeReady, esHiloPorId)) {
			auxiliar = list_remove_by_condition(programa->listaDeReady, esHiloPorId);
			auxiliar->estado = EXIT;
			list_add(listaExit, auxiliar);
		}

		else if(list_any_satisfy(listaNuevos, esHiloPorId)) {
			auxiliar = list_remove_by_condition(listaNuevos, esHiloPorId);

		}

		else {
			log_error(log, "El hilo a hacerle suseClose no se encuentra en memoria");

		}

		programasEnMemoria --;
		list_remove_by_condition(LISTAENEJECUCION, esHiloPorId);


	}

}
//void freeHilo(t_hilo* hilo) {
//	free(hilo->semaforos);
//	free(hilo);
//}

//t_programa* crearPrograma(int id) {
//	t_programa* nuevo = malloc(sizeof(t_programa));
//	nuevo->id = id;
//	nuevo->listaDeHilos = list_create();
//	nuevo->listaDeReady = list_create();
//	nuevo->enEjecucion = NULL;
//
//	return nuevo;
//}

//void freePrograma(t_programa* programa) {
//	list_destroy_and_destroy_elements(programa->listaDeHilos, freeHilo());
//	queue_destroy_and_destroy_elements(programa->colaDeReady, freeHilo());
//	free(programa->enEjecucion);
//	free(programa);
//}

//void sigterm(int sig) {
//	log_info(log, "Se interrumpió la ejecución del proceso");
//	log_destroy(log);
//}


//void destruirPrograma(t_programa* programa) {
//	auxiliarParaId = programa->id;
//	list_destroy_and_destroy_elements(programa->listaDeHilos, destruirHilo);
//	list_remove_by_condition(listaDeProgramas, esProgramaPorId);
//	free(programa);
//}


//void* handler(void* socketConectado) {
//	int socket = *(int*)socketConectado;
//	t_programa* programa = crearPrograma(socket);
//	list_add(listaDeProgramas, programa);
//
//
//	t_message* bufferLoco;
//
//	while((bufferLoco = recv_message(socket))->head < 10) { // HAY CODIGOS HASTA 7 + Prueba, por eso menor a 7.HAY QUE AGREGAR UNA COLA DE ESPERA
//		log_info(log, "Se recibio un mensaje en programa: %i", programa->id);
//		int threadId = *(int*)bufferLoco->content;
//		char* stringAuxiliar;
//		double numeroAux;
//		t_header header = bufferLoco->head;
//		size_t tamanio = bufferLoco->size;
//
//		switch(bufferLoco->head) {
//			case SUSE_CREATE:
//				suseCreate(threadId, programa);
//				break;
//
//			case SUSE_SCHEDULE_NEXT:
//					numeroAux = (double)threadId; //En este caso threadId se comporta como el contenedor de el tiempo real para SJF
//
//					if(programa->enEjecucion != NULL) {
//						estimarDuracionHilo(programa->enEjecucion, numeroAux);
//					}
//
//					cargarHilosAReady();
//					suseScheduleNext(programa);
//
//
//
//
//				break;
//
//			case SUSE_WAIT:
//				if((bufferLoco = recv_message(socket))->head == SUSE_CONTENT) {
//					stringAuxiliar = malloc(sizeof(bufferLoco->content));
//					stringAuxiliar = (char*)bufferLoco->content;
//					suseWait(threadId, stringAuxiliar, programa);
//					free(stringAuxiliar);
//				}
//				else {
//					log_error(log, "Se recibió en SUSE_SIGNAL un mensaje que no corresponde a este lugar");
//					free(stringAuxiliar);
//				}
//				break;
//
//			case SUSE_SIGNAL:
//				if((bufferLoco = recv_message(socket))->head == SUSE_CONTENT) {
//					stringAuxiliar = malloc(sizeof(bufferLoco->content));
//					stringAuxiliar = (char*)bufferLoco->content;
//					suseSignal(threadId, stringAuxiliar, programa);
//					free(stringAuxiliar);
//				}
//				else {
//					log_error(log, "Se recibió en SUSE_SIGNAL un mensaje que no corresponde a este lugar");
//					free(stringAuxiliar);
//				}
//				break;
//
//			case SUSE_JOIN:
//				//suseJoin();
//				break;
//
//			case SUSE_CLOSE:
//					suseClose(threadId, programa);
//				break;
//
//			case SUSE_CONTENT:
//				log_error(log, "SE RECIBIO AL HANDLER UN MENSAJE CON CONTENIDO DESTINADO A OTRO LUGAR");
//				break;
//
//			case TEST:
//				printf("El header es: %i --- El contenido es: %i --- Su tamaño es: %zu\n", header, threadId, tamanio);
//				break;
//
//			default:
//				log_error(log, "La instruccion no es correcta\n");
//				break;
//		}
//
//	}
//
//	log_info(log, "Se ha producido un problema de conexión y el hilo programa se dejará de planificar: %i.\n", bufferLoco->head);
//	destruirPrograma(programa);
//	free_t_message(bufferLoco);
//	close(socket);
//	return NULL;
//}

//int main() {
//	signal(SIGINT, sigterm);
//	log = log_create("log", "log_suse.txt", true, LOG_LEVEL_DEBUG);
//	load_suse_config();
//	cargarSemaforos();
//
//	listaDeProgramas = list_create();
//	listaDeBloqueados = list_create();
//	LISTAENEJECUCION = list_create();
//	listaNuevos = list_create();
//	listaExit = list_create();
//
//	int socketDelCliente;
//	struct sockaddr direccionCliente;
//	unsigned int tamanioDireccion = sizeof(direccionCliente);
//	int servidor = init_server(listen_port);
//
//
//	while((socketDelCliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion))>=0) {
//		pthread_t threadId;
//		log_info(log, "Se ha aceptado una conexion: %i\n", socketDelCliente);
//		if((pthread_create(&threadId, NULL, handler, (void*) &socketDelCliente)) < 0) {
//			log_info(log, "No se pudo crear el hilo");
//			return 1;
//		} else {
//			log_info(log, "Handler asignado\n");
//			//pthread_join(threadId, NULL);
//		}
//
//
//	}
//	if(socketDelCliente < 0) {
//		log_info(log, "Falló al aceptar conexión");
//	}
//
//	close(servidor);
//
//	return 0;
//}
