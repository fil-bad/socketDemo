//
// Created by filippo on 25/09/18.
//

#include "socketConnect.h"

/// GLOBAL FUNCTION
connection* initSocket(int port, char* IP){

    connection *con = malloc(sizeof(connection));
    con->ds_sock = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char*)&con, sizeof(con));
    con->sock.sin_family = AF_INET;
    con->sock.sin_port = htons(port);
    if (strcmp(IP, "INADDR_ANY") == 0) con->sock.sin_addr.s_addr = INADDR_ANY;
    else con->sock.sin_addr.s_addr = inet_addr("127.0.0.1");
    return con;
}

void freeConnection(connection* con){
    free(con);
}