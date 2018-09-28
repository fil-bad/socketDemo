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

int writePack(int ds_sock, mail *pack) //dentro il thArg deve essere puntato un mail
{
    /// la funzione si aspetta che il buffer non sia modificato durante l'invio
    ssize_t bWrite = 0;

    ssize_t dimPack = sizeof(metadata) + pack->md.dim;
    do{
        bWrite = write(ds_sock,pack+bWrite, dimPack - bWrite);
    } while (dimPack-bWrite > 0);

    return 0;
}

int readPack(int ds_sock, mail *pack) //todo: implementare controllo sulle read
{
    ssize_t bRead = 0;
    metadata *mDServ = malloc(sizeof(metadata));

    int dimMD = sizeof(metadata); // dimensione metadata, serve per prendere dati corretti in read;
    do{
        bRead = read(ds_sock,mDServ+bRead, sizeof(mDServ)-bRead);
    } while (dimMD-bRead > 0);
    printf("Dim pack = %ld, Type = %d sender = %s, whoOrWhy = %s\n", mDServ->dim, mDServ->type,mDServ->sender,mDServ->whoOrWhy);

    size_t dimMex = mDServ->dim;

    pack->mex = malloc(dimMex);

    bRead = 0; //rimetto a zero per la nuova lettura
    do{
        bRead = read(ds_sock,pack->mex+bRead, sizeof(pack->mex)-bRead);
    } while (dimMex-bRead > 0);

    printf("Message = %s\n",(char*)pack->mex);

    return 0;
}

void freeConnection(connection* con){
    free(con);
}

void *threadUser(thConnArg * argTh){

    mail *pack = malloc(sizeof(mail));

    loginServerSide(argTh->con.ds_sock, pack);
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

int loginServerSide(int ds_sock, mail *pack){
    // vedere sendfile() su man, potrebbe servire per il login
    printf("Utente in fase di collegamento; socket univoco:%d\n",ds_sock);

    readPack(ds_sock,pack);

    printf("Utente = %s\n",(char *)pack->mex);
    printf("Cerco corrispondenza utente e chat associate.\n");


    writePack(ds_sock,pack);


    return 0;
}