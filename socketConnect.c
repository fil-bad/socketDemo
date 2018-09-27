//
// Created by filippo on 25/09/18.
//

#include "socketConnect.h"

/// GLOBAL FUNCTION
connection* initSocket(u_int16_t port, char* IP)
{
    connection *con = malloc(sizeof(connection));
    con->ds_sock = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char*)&con, sizeof(con));
    con->sock.sin_family = AF_INET;
    con->sock.sin_port = htons(port);
    if (strcmp(IP, "INADDR_ANY") == 0) con->sock.sin_addr.s_addr = INADDR_ANY;
    else con->sock.sin_addr.s_addr = inet_addr("127.0.0.1");
    return con;
}

int writePack(mail *pack)
{
    /// la funzione si aspetta che il buffer non sia modificato durante l'invio
    //size_t lenMess = pack->md.dim+ sizeof(metadata);
    //char buffer[lenMess];
    //memcpy(buffer, &pack->md, sizeof(metadata));
    //memcpy(buffer + sizeof(metadata), pack->mex, pack->md.dim);
}

int readPack()
{

}

void freeConnection(connection* con){
    free(con);
}

void *threadUser(thConnArg * argTh){

    mail *pack = malloc(sizeof(mail));
    argTh->arg = pack;


    loginServerSide(argTh);
}




///Server FUNCTION

int initServer(connection *c, int coda)
{
    if(bind(c->ds_sock, (struct sockaddr *) &c->sock, sizeof(c->sock)))
    {
        perror("Bind failed, cause:");
        return -1;
    }
    if(listen(c->ds_sock, coda)){
        perror("Listen failed, cause:");
        return -1;
    }
    return 0;
}

int acceptCreate(connection *c,  void* (*threadUser)(void *),void *arg)
{
    // Si suppone che arg sia stata precedentemente malloccata
    connection *conNew;
    unsigned int len = sizeof(conNew->sock);
    conNew=malloc(sizeof(connection));
    conNew->ds_sock = accept(c->ds_sock,(struct sockaddr *) &conNew->sock, &len);
    if (conNew->ds_sock == -1){
        perror("Accept client error; cause:");
        close(conNew->ds_sock);
        return -1;
    }
    pthread_t tid;
    thConnArg *argTh=malloc(sizeof(thConnArg));
    argTh->arg = arg;
    argTh->con = *conNew;
    pthread_create(&tid,NULL,threadUser,argTh);
    return 0;
}



///Client FUNCTION

int initClient(connection *c)
{
    if (connect(c->ds_sock,(struct sockaddr *) &c->sock,sizeof(c->sock)))
    {
        perror("Connect error; cause:");
        close(c->ds_sock);
        return -1;
    }
    return 0;
}

int loginServerSide(thConnArg *thArg){
    // vedere sendfile() su man, potrebbe servire per il login

    ssize_t bRead = 0;
    metadata *mDServ = malloc(sizeof(metadata));

    printf("Utente in fase di collegamento; socket univoco:%d\n",thArg->con.ds_sock);
    int dimMD = sizeof(metadata); // dimensione metadata, serve per prendere dati corretti in read;
    do{
        bRead = read(thArg->con.ds_sock,mDServ+bRead, sizeof(mDServ-bRead));
    } while (dimMD-bRead > 0);



    return 0;
}