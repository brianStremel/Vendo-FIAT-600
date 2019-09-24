/*
 * nodes.c
 *
 *  Created on: 13 sep. 2019
 *      Author: utnso
 */
#include "nodes.h"

int lastchar(const char* str, char chr){
	if ( ( str[strlen(str)-1]  == chr) ) return 1;
	return 0;
}


GFile create_GFile(char status, char file_name[71], int32_t root, int32_t size,
		char creation_date[8], char modification_date[8]) {

	GFile gFile = { .status = status, .root = root, .size = size };

	strcpy(gFile.file_name, file_name);
	strcpy(gFile.creation_date, creation_date);
	strcpy(gFile.modification_date, modification_date);
//	for(int i=0; i<1000; i++){
//		gFile.blocks_ptr[i] = blocks_ptr[i];
//	}

	return gFile;
}

GHeader create_sac_header(char identifier[3], int32_t version,
		int32_t init_block, int32_t bit_map_size) {
	GHeader header = { .version = version, .init_block = init_block,
			.bit_map_size = bit_map_size, };

	strcpy(header.identifier, identifier);

	return header;
}

int search_node(const char* path) {
	if (!strcmp(path, "/"))
		return 0;
	int res;
	char* name = get_name(path);
	char* directory = get_directory(path);

	int root = search_node(directory);
	int index;
	for (index = 1;
			(nodes_table[index].root != root
					|| strcmp(nodes_table[index].file_name, name) != 0)
					&& index < BLOCKS_NODE; index++);

	if (index >= BLOCKS_NODE)
		res = -1;
	else {
		res = index;
	}

	free(name);
	free(directory);
	return res;
}

char* get_name(const char* path) {
	char** aux = string_split((char*) path, "/");
	int i;
	for (i = 0; aux[i] != NULL; i++)
		;
	char * name = malloc(strlen(aux[i - 1]) + 1);
	strcpy(name, aux[i - 1]);
	name[strlen(aux[i - 1])] = '\0';
	for (int j = 0; aux[j] != NULL; j++)
		free(aux[j]);
	free(aux);
	return name;
}

char* get_directory(const char* path) {
	char* file = get_name(path);
	char* directory = malloc(strlen(path) + 1);
	strcpy(directory, path);
	if (lastchar(path, '/')) {
		directory[strlen(directory)-1] = '\0';
	}
	int i = strlen(directory) - strlen(file);
	directory[i] = '\0';

	free(file);
	return directory;
}

int search_first_free_node(){
	int i;
	for(i= 0; nodes_table[i].status !=0 && i < BLOCKS_NODE; i++);
	if(i>=BLOCKS_NODE)
		return -1;
	else
		return i;
}

int search_and_test_first_free_block(){
	int res = -1;
	for(int i = 0; i < bitmap->size*8 && res!= -1; i++){
		if(bitarray_test_bit(bitmap,i) == 0){
			bitarray_set_bit(bitmap,i);
			res = i;
		}
	}
	return res;
}
