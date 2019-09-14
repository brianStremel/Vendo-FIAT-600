/*
 * sac_config.h
 *
 *  Created on: 13 sep. 2019
 *      Author: utnso
 */

#ifndef SAC_CONFIG_H_
#define SAC_CONFIG_H_
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/temporal.h>

/*
 * Es un struct por si después agregamos más cosas al archivo de configuración
 */
typedef struct{
	int listen_port;
} sac_config;


sac_config* sac_load_config(char* path);
#endif /* SAC_CONFIG_H_ */
