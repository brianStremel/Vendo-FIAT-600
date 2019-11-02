/*
 * protocol.h
 *
 *  Created on: 8 sep. 2019
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef PROTOCOL_H_
#define PROTOCOL_H_


typedef enum{
	SUSE_INIT = 0,
	SUSE_CREATE = 1,
	SUSE_SCHEDULE_NEXT = 2,
	SUSE_WAIT = 3,
	SUSE_SIGNAL = 4,
	SUSE_JOIN = 5,
	TEST = 6,
	HEAD,
	GET_ATTR,
	READ_LINK,
	CREATE,
	UNLINK,
	MKDIR,
	OPENDIR,
	RMDIR,
	READDIR,
	MKNODE,
	READ,
	WRITE,
	OK,
	DIRECTORY_NOT_FOUND,
	DIR_NAME,
	FILE_ALREADY_EXISTS,
	FILE_NOT_FOUND,
	NO_CONNECTION = 100,
	ERROR_RECV = 101,
	HI_PLEASE_BE_MY_FRIEND = 102,
	ERROR_MESSAGE = 103
}t_header;

typedef struct{
	t_header head;
	size_t size;
	void* content;
}t_message;

t_message* create_t_message(t_header head, size_t size, void* content);

int send_message(int socket, t_header head,const void* content, size_t size);

t_message* recv_message(int socket);

void free_t_message(t_message* message);

t_message* no_connection();
t_message* error_recv();

int send_header(int socket, t_header head);

t_header recv_header(int socket);

#endif /* PROTOCOL_H_ */
