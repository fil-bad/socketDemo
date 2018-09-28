//
// Created by filippo on 25/09/18.
//

#ifndef SOCKETDEMO_SOCKETCONNECT_H
#define SOCKETDEMO_SOCKETCONNECT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <poll.h>

#define MAX_DIM 1024

typedef struct metadata_{
    size_t dim;
    int type;
    char sender[28];
    char whoOrWhy[24];
}metadata;

typedef struct mail_{
    metadata md;
    void *mex;
} mail;

typedef struct connection_{
    int ds_sock;
    struct sockaddr_in sock;
} connection;

typedef struct thConnArg_{
    connection con;
    void *arg;
}thConnArg;

// PROTOTIPI DI FUNZIONE:

// GLOBALI

connection* initSocket(u_int16_t port, char* IP);
void freeConnection(connection* con);

void *threadUser(thConnArg *);

int readPack(int ds_sock, mail *pack);  // queste due funzioni prendono il pacchetto thread-specifico
int writePack(int ds_sock, mail *pack); // ma all'interno contengono la struct mail con i dati

///Server FUNCTION

int initServer(connection *connection, int coda);
int acceptCreate(connection *connection,  void* (*threadUser)(void *),void *argFx);
int loginServerSide(int sock, mail *pack);

///Client FUNCTION
int initClient(connection *c);
int loginUserSide(int sock, mail *pack);

#endif //SOCKETDEMO_SOCKETCONNECT_H
