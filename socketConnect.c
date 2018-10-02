//
// Created by filippo on 25/09/18.
//

#include "socketConnect.h"

/// GLOBAL FUNCTION
connection* initSocket(u_int16_t port, char* IP)
{
    connection *con = malloc(sizeof(connection));

    con->ds_sock = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&con->sock, sizeof(con->sock));
    con->sock.sin_family = AF_INET;
    con->sock.sin_port = htons(port);
    if (strcmp(IP, "INADDR_ANY") == 0) {
        con->sock.sin_addr.s_addr = INADDR_ANY;
    }
    else {
        con->sock.sin_addr.s_addr = inet_addr("127.0.0.1");
    }
    return con;
}

int writePack(int ds_sock, mail *pack) //dentro il thArg deve essere puntato un mail
{
    /// la funzione si aspetta che il buffer non sia modificato durante l'invio
    ssize_t bWrite = 0;

    ssize_t dimPack = sizeof(metadata) + pack->md.dim;
    do{
        bWrite += write(ds_sock,pack+bWrite, sizeof(metadata) - bWrite);
    } while (sizeof(metadata)-bWrite != 0);

    bWrite = 0;
    do{
        bWrite += write(ds_sock,pack->mex+bWrite, pack->md.dim - bWrite);
    } while (pack->md.dim-bWrite != 0);

    return 0;
}

int readPack(int ds_sock, mail *pack) //todo: implementare controllo sulle read
{
    ssize_t bRead = 0;

    do{
        bRead += read(ds_sock,&pack->md+bRead, sizeof(metadata)-bRead);
    } while (sizeof(metadata)-bRead != 0);

    printPack(pack);

    size_t dimMex = pack->md.dim;

    if (dimMex == 0) {
        pack->mex = NULL;
        return 0;
    }

    pack->mex = malloc(dimMex);

    bRead = 0; //rimetto a zero per la nuova lettura
    do{
        bRead += read(ds_sock,pack->mex+bRead, dimMex-bRead);
    } while (dimMex-bRead != 0);

    printf("Message = %s\n",(char*)pack->mex);

    return 0;
}

void freeConnection(connection* con){
    free(con);
}



int fillPack(mail *pack, int type, char *sender, char *whoOrWhy, void *mex, int len){
    pack->md.dim = len;
    pack->md.type = type;
    memcpy(pack->md.whoOrWhy, whoOrWhy, strlen(whoOrWhy));
    memcpy(pack->md.sender, sender, strlen(sender));
    pack->mex = mex;
    return 0;
}

void printPack(mail *pack)
{
    printf("######[][]I METADATI SONO[][]######\n");
    printf("Dim pack = %ld\n",pack->md.dim);
    printf("Type = %d\n",pack->md.type);
    printf("Sender = %s\n",pack->md.sender);
    printf("whoOrWhy = %s\n",pack->md.whoOrWhy);
    printf("------[][]IL MESSAGGIO[][]------\n");
    printf("TEXT :\n--> %s\n",pack->mex); //non sempre stringa
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

int loginServerSide(int ds_sock, mail *pack){
    // vedere sendfile() su man, potrebbe servire per il login
    printf("Utente in fase di collegamento; socket univoco:%d\n",ds_sock);

    readPack(ds_sock,pack);

    printf("Utente = %s\n",(char *)pack->mex);
    printf("Cerco corrispondenza utente e chat associate.\n");


    writePack(ds_sock,pack);


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

int loginUserSide(int ds_sock, mail *pack){

    char buffUser[24];
    char buffPass[28];
    memset(buffUser, 0, sizeof(buffUser));
    memset(buffPass, 0, sizeof(buffPass));

    printf("Inserire credenziali per login.\nUtente: ");
    scanf("%s",buffUser);
    fflush(stdin);
    printf("\nPassword:");
    scanf("%s",buffPass);
    fflush(stdin);
    printf("\n");

    if (fillPack(pack, 0, buffUser, buffPass, NULL,0) == -1){
        return(-1);
    }

    writePack(ds_sock, pack);

    readPack(ds_sock,pack);



    return 0;
}