#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "socketConnect.h"

typedef struct thUserServ_{
    int id;
    char name[128];
}thUserServ;


void helpProject(void);
int serverDemo(int argc, char *argv[]);
int clientDemo(int argc, char *argv[]);
void *thUserServer(thConnArg *);




connection *con;

int main(int argc, char *argv[])
{

    if (argc >= 4)
    {
        if(strcmp(argv[1], "s") == 0)
        {

            serverDemo(argc, argv);
            return 0;
        }

        else if(strcmp(argv[1], "c") == 0)
        {
            clientDemo(argc, argv);
            return 0;
        }
    }
    helpProject();

    return 0;
}

int serverDemo(int argc, char *argv[])
{
    printf("Server in avvio\n");
    con = initSocket(atoi(argv[2]),"INADDR_ANY");
    printf("Socket inizializzato\n");
    if(initServer(con,atoi(argv[3])) == -1){
        exit(-1);
    }
    printf("Server avviato\n");
    pthread_t tid;
    thUserServ *arg;
    int i = 0;
    while(1){
        arg = malloc(sizeof(thUserServ));
        arg->id = i;
        if(acceptCreate(con, thUserServer, arg) == -1) {
            exit(-1);
        }
        i++;
    }

    return 0;
}

void *thUserServer(thConnArg *argTh){
    thUserServ *info = argTh->arg;
    printf("TH creato\nId = %d\n", info->id);

    mail *packRecive= malloc(sizeof(mail));
    printf("mi metto in ascolto\n");

    do
    {
        readPack(argTh->con.ds_sock,packRecive);
        printf("Numero byte pacchetto: %d\n",packRecive->md.dim);
        
        printf("Stringa da client: %s\n", packRecive->mex);
    }
    while(strcmp(packRecive->mex,"quit") != 0);
    printf("TH user %d in chiusura\n", info->id);
    close(argTh->con.ds_sock);
    free(info);
    free(argTh);
    pthread_exit(0);
    
    //todo: testare fase login
    /*
    mail *pack = malloc(sizeof(mail));

    loginServerSide(argTh->con.ds_sock, pack);
     */

    
}

int clientDemo(int argc, char *argv[])
{
    con = initSocket(atoi(argv[2]),argv[3]);

    if(initClient(con) == -1){
        exit(-1);
    }
    mail *packSend = malloc(sizeof(mail));
    //packSend->mex=malloc(4096);

    do
    {
        free(packSend->mex);
        antiSegFault:
        scanf("%m[^\n]", &packSend->mex);
        fflush(stdin);

        if (packSend->mex == NULL) goto antiSegFault;
        packSend->md.dim=strlen(packSend->mex)+1;
        packSend->md.type=1;
        strncpy(packSend->md.sender,"fattonia",28);
        strncpy(packSend->md.whoOrWhy,"fattone",24);
        printf("Stringa inserita: %s\n", packSend->mex);
        writePack(con->ds_sock,packSend);
        printPack(packSend);


    }
    while(strcmp(packSend->mex,"quit") != 0);
    free(packSend->mex);
    free(packSend);
    close(con->ds_sock);
    return 0;
}

void helpProject(void)
{
    printf("I parametri inseribili sono:\n");
    printf("\tServer command:\n");
    printf("\ts [port] [coda]\t\tentro in modalita' server alla porta specificata\n");
    printf("\tClient command:\n");
    printf("\tc [port] [IP]\t\tentro in modalita' client alla porta e IP specificati\n");
}