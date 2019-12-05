#include "utils.h"
#include "estructuras.h"
#include "muse.h"

t_list* obtenerListaSegmentosPorId(char* idPrograma){
	bool esElPrograma(Programa* programa){
		return string_equals_ignore_case(programa->id,idPrograma);
	}
	pthread_mutex_lock(&mut_listaProgramas);
	Programa* programaEncontrado = list_find(listaProgramas,(void*)esElPrograma);
	pthread_mutex_unlock(&mut_listaProgramas);
	if(programaEncontrado == NULL){
		return NULL;
	}
	return programaEncontrado->segmentos;
}

t_list* obtenerMapeoExistente(char* path,int tamanio){
	t_list* paginas = NULL;
	void closure(Mapeo* mapeo){
		if(string_equals_ignore_case(mapeo->path,path) && mapeo->tamanio == tamanio){
			paginas = mapeo->paginas;
			mapeo->contador++;
		}
	}
	pthread_mutex_lock(&mut_mapeos);
	list_iterate(listaMapeos,(void*)closure);
	pthread_mutex_unlock(&mut_mapeos);
	return paginas;
}

Segmento* segmentoAlQuePertenece(t_list* listaSegmentos, uint32_t direccion){
	for(int i = 0; i < list_size(listaSegmentos); i++){
		if(direccion - ((Segmento*)list_get(listaSegmentos,i))->base_logica < ((Segmento*)list_get(listaSegmentos,i))->tamanio){
			return ((Segmento*)list_get(listaSegmentos,i));
	}
	return NULL;
}

void paginasMapEnMemoria(int direccion, int tamanio, Segmento* segmentoEncontrado){
	int primerPag = direccion / TAMANIO_PAGINA;
	int offset = direccion % TAMANIO_PAGINA;
	int ultimaPag = techo((direccion+tamanio) / TAMANIO_PAGINA) - 1;

	if(tienePaginasNoCargadasEnMemoria(segmentoEncontrado,primerPag,ultimaPag)){
		int cantidadPags = ultimaPag - primerPag + 1;
		int bytesPorLeer = cantidadPags * TAMANIO_PAGINA;
		int relleno = bytesPorLeer - offset - tamanio;
		void* bloqueRelleno = generarRelleno(relleno);
		int ultimaPaginaLista = segmentoEncontrado->paginas->elements_count-1; // is that allowed, WHAT THE FUCK IS THIS ALLOWED

		void* buffer = malloc(bytesPorLeer);
		FILE* archivo = fopen(segmentoEncontrado->path_mapeo,"rb");
		fread(buffer,TAMANIO_PAGINA,cantidadPags,archivo);
		fclose(archivo);
		int puntero = primerPag * TAMANIO_PAGINA;
		int i = primerPag;
		while(i <= ultimaPag){
			Pagina* pag = (Pagina*)list_get(segmentoEncontrado,i);
			if(pag->bit_marco == NULL && pag->bit_swap == NULL){
				pag->bit_marco = asignarMarcoNuevo();
				pag->bit_marco->bit_uso = true;
				pag->bit_marco->bit_modificado = true;
				pag->presencia = true;

				void* punteroMarco = obtenerPunteroAMarco(pag);
				if(pag->num_pagina == ultimaPaginaLista){
					memcpy(punteroMarco, buffer + puntero, TAMANIO_PAGINA - relleno);
					memcpy(punteroMarco + TAMANIO_PAGINA - relleno, bloqueRelleno,relleno);
				} else {
					memcpy(punteroMarco,buffer + puntero, TAMANIO_PAGINA);
				}
			}

			i++;
			puntero += TAMANIO_PAGINA;
			bytesPorLeer -= TAMANIO_PAGINA;
		}
		free(buffer);
		free(bloqueRelleno);
	}
}

bool tienePaginasNoCargadasEnMemoria(Segmento* segmento, int pagInicial, int pagFinal){
	for(int i = 0; i < list_size(segmento->paginas); i++){
		Pagina* pag = ((Pagina*)list_get(segmento->paginas,i));
		if(pag->num_pagina >= pagInicial && pag->num_pagina <= pagFinal){
			if(pag->bit_marco == NULL && pag->bit_swap == NULL){
				return true;
			}
		}
		return false;
	}
}


void* obtenerPunteroAMarco(Pagina* pag){
	if(!pag->presencia){
		if(pag->bit_swap != NULL){
			void* paginaVictima = malloc(TAMANIO_PAGINA);
			memcpy(paginaVictima, posicionInicialSwap + pag->bit_swap->pos * TAMANIO_PAGINA, TAMANIO_PAGINA);
			//copio lo que esta en swap a un puntero temporal
			//pag->bit_swap->esta_ocupado = false;
			pag->bit_swap = NULL;
			BitMemoria* bit = asignarMarcoNuevo();
			pag->bit_marco = bit;
			pag->presencia = true;
			//pego la pag en mem
			memcpy(posicionInicialMemoria + bit->pos * TAMANIO_PAGINA, paginaVictima, TAMANIO_PAGINA);
		}
		else{
			log_info(logger,"I ain't no hollaback girl");
		}
	}
	pag->bit_marco->bit_uso = true;
	return posicionInicialMemoria + pag->bit_marco->pos * TAMANIO_PAGINA;
}

BitMemoria* asignarMarcoNuevo(){
	BitMemoria* bit = buscarBitLibreMemoria();
	if(bit == NULL){
		bit = ejecutarClockModificado();
	}
	return bit;
}

BitMemoria* buscarBitLibreMemoria(){
	pthread_mutex_lock(&mut_bitmap);
	//si encuentro uno, ya le pongo como que esta usado
	bool bit_libre(BitMemoria* bit){
		return !bit->esta_ocupado;
	}
	BitMemoria* bitEncontrado = list_find(bitmap->bits_memoria,(void*)bit_libre);
	if(bitEncontrado!=NULL){
		bitEncontrado->esta_ocupado = true;
		bitEncontrado->bit_modificado = true;
		bitEncontrado->bit_uso = true;
	}
	pthread_mutex_unlock(&mut_bitmap);
	return bitEncontrado;
}

void paginasMapEnMemoria(int direccion, int tamanio, Segmento* segmentoEncontrado){
	int primerPag = direccion / TAMANIO_PAGINA;
	int offset = direccion % TAMANIO_PAGINA;
	int ultimaPag = techo((direccion+tamanio) / TAMANIO_PAGINA) - 1;

	if(tienePaginasNoCargadasEnMemoria(segmentoEncontrado,primerPag,ultimaPag)){
		int cantidadPags = ultimaPag - primerPag + 1;
		int bytesPorLeer = cantidadPags * TAMANIO_PAGINA;
		int relleno = bytesPorLeer - offset - tamanio;
		void* bloqueRelleno = generarRelleno(relleno);
		int ultimaPaginaLista = segmentoEncontrado->paginas->elements_count-1; // is that allowed, WHAT THE FUCK IS THIS ALLOWED

		void* buffer = malloc(bytesPorLeer);
		FILE* archivo = fopen(segmentoEncontrado->path_mapeo,"rb");
		fread(buffer,TAMANIO_PAGINA,cantidadPags,archivo);
		fclose(archivo);
		int puntero = primerPag * TAMANIO_PAGINA;
		int i = primerPag;
		while(i <= ultimaPag){
			Pagina* pag = (Pagina*)list_get(segmentoEncontrado,i);
			if(pag->bit_marco == NULL && pag->bit_swap == NULL){
				pag->bit_marco = asignarMarcoNuevo();
				pag->bit_marco->bit_uso = true;
				pag->bit_marco->bit_modificado = true;
				pag->presencia = true;

				void* punteroMarco = obtenerPunteroAMarco(pag);
				if(pag->num_pagina == ultimaPaginaLista){
					memcpy(punteroMarco, buffer + puntero, TAMANIO_PAGINA - relleno);
					memcpy(punteroMarco + TAMANIO_PAGINA - relleno, bloqueRelleno,relleno);
				} else {
					memcpy(punteroMarco,buffer + puntero, TAMANIO_PAGINA);
				}
			}

			i++;
			puntero += TAMANIO_PAGINA;
			bytesPorLeer -= TAMANIO_PAGINA;
		}
		free(buffer);
		free(bloqueRelleno);
	}
}

bool tienePaginasNoCargadasEnMemoria(Segmento* segmento, int pagInicial, int pagFinal){
	for(int i = 0; i < list_size(segmento->paginas); i++){
		Pagina* pag = ((Pagina*)list_get(segmento->paginas,i));
		if(pag->num_pagina >= pagInicial && pag->num_pagina <= pagFinal){
			if(pag->bit_marco == NULL && pag->bit_swap == NULL){
				return true;
			}
		}
		return false;
	}
}

void unmapear(Segmento* segmento, char* idPrograma){
	bool esPrograma(Programa* prog){
		return string_equals_ignore_case(prog->id,idPrograma);
	}

	bool esSegmento(Segmento* seg){
		if(seg->es_mmap){
			return seg->base_logica == segmento->base_logica;
		}
		return false;
	}

	bool encontrarMapeo(Mapeo* mapeo){
		return string_equals_ignore_case(mapeo->path,segmento->path_mapeo);
	}

	pthread_mutex_lock(&mut_mapeos);
	Mapeo* mapeo = NULL;
	for(int i = 0; i < list_size(listaMapeos); i++){
		if(string_equals_ignore_case(((Mapeo*)list_get(listaMapeos,i))->path,segmento->path_mapeo)){
			mapeo = list_get(listaMapeos,i);
			break;
		}
	}

	if(mapeo != NULL){
		mapeo->contador--;
		//saco el segmento de la lista de segmentos del programa
		pthread_mutex_lock(&mut_listaProgramas);
		Programa* prog = list_find(listaProgramas,(void*)esPrograma);
		pthread_mutex_unlock(&mut_listaProgramas);
		list_remove_by_condition(prog->segmentos,(void*)esSegmento);

		if(mapeo->contador==0){//si no quedan mas referenciandolo se elimina
			list_remove_and_destroy_by_condition(listaMapeos,(void*)encontrarMapeo,(void*)destruirMapeo);
		}
	}

	pthread_mutex_unlock(&mut_mapeos);
}

void destruirMapeo(Mapeo* mapeo){
	free(mapeo->path);
	list_destroy_and_destroy_elements(mapeo->paginas,(void*)destruirPagina);
	free(mapeo);
}

void destruirPagina(Pagina* pag){
	if(pag->bit_marco != NULL){
		pag->bit_marco->esta_ocupado = false;
		pag->bit_marco = NULL;
	}
	if(pag->bit_swap != NULL){
		pag->bit_swap->esta_ocupado = false;
	}
	free(pag);
}

void* generarRelleno(int relleno){
	void* voidRelleno = malloc(relleno);
	char* caracter = malloc(1);
	char barraCero = '\0';
	memcpy(caracter,&barraCero,1);
	int puntero = 0;
	for(int i = 0; i < relleno; i++, puntero++){
		memcpy(voidRelleno+puntero,(void*)barraCero,1);
	}
	free(caracter);
	return voidRelleno;
}

Segmento* obtenerSegmentoPorDireccion(t_list* listaSegmentos, uint32_t direccion){
	bool buscarSegmento(Segmento* seg){
		if(direccion - seg->base_logica < seg->tamanio){
			return true;
		}
		return false;
	}

	return list_find(listaSegmentos,(void*)buscarSegmento);
}

int techo(double d){
	if(d-(int)d != 0){
		return (int)d + 1;
	}
	else{
		return (int)d;
	}
}

uint32_t calcularFramesNecesarios(uint32_t tam){
	int frames_necesarios = 0;
	float aux = ((float)(tam) / (float)TAMANIO_PAGINA);
	do{
		frames_necesarios++;
	}while(aux > frames_necesarios);
	return frames_necesarios;
}

Segmento* obtenerSegmentoParaOcupar(t_list* listaSegmentos,uint32_t tamanio){

	bool tieneHeapDisponible(InfoHeap* heap){
		if(heap->libre && heap->espacio >= tamanio){
			return true;
		}
		return false;
	}

	bool findSegmentoConEspacio(Segmento* seg){
		if(!seg->es_mmap){
			return list_any_satisfy(seg->status_metadata,(void*)tieneHeapDisponible);
		} else {
			return false;
		}
	}
	return list_find(listaSegmentos,(void*)findSegmentoConEspacio);
}

InfoHeap* obtenerHeapConEspacio(Segmento* segmento, uint32_t tamanio){
	for(int i = 0; i < list_size(segmento->status_metadata); i++){
		if(((InfoHeap*)list_get(segmento->status_metadata, i))->libre && ((InfoHeap*)list_get(segmento->status_metadata, i))->espacio >= tamanio + sizeof(HeapMetadata)){
			return list_get(segmento->status_metadata, i);
		}
	}
}

Segmento* obtenerUltimoSegmento(t_list* listaSegmentos){
	return list_get(listaSegmentos, list_size(listaSegmentos) - 1);
}

uint32_t obtenerBaseLogicaNuevoSegmento(t_list* listaSegmentos){
	if(list_is_empty(listaSegmentos)){
		return 0;
	}
	Segmento* ultimoSegmento = obtenerUltimoSegmento(listaSegmentos);
	return ultimoSegmento->base_logica + ultimoSegmento->paginas->elements_count * TAMANIO_PAGINA;
}



