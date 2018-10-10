#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "socketConnect.h"

typedef struct thUserServ_{
    int id;
    char name[128];
}thUserServ;


void helpProject(void);
int serverDemo(int argc, char *argv[]);
int clientDemo(int argc, char *argv[]);
void *thUserServer(thConnArg *);
void *thrServRX(thConnArg *);
void *thrServTX(thConnArg *);
void *thrUserRX(mail *);
void *helperClient(int);

sem_t sem[2]; // test sincronia
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
    if (con == NULL){
        exit(-1);
    }

    printf("Socket inizializzato\n");
    if(initServer(con,atoi(argv[3])) == -1){
        exit(-1);
    }

    printf("Server avviato\n");
    pthread_t tid;
    thUserServ *arg;
    int i = 0;


    /// parte con semafori di test
    sem_init(&sem[0],0,1);
    sem_init(&sem[1],0,0);



    while(1){
        arg = malloc(sizeof(thUserServ));
        printf("arg = %p creato.\n", arg);
        arg->id = i;
        if(acceptCreate(con, thUserServer, arg) == -1)
        {
            exit(-1);
        }
        i++;
    }

    return 0;
}

void *thUserServer(thConnArg *argTh){
    thUserServ *info=argTh->arg;
    printf("TH creato\nId = %d\n", info->id);

    mail *packRecive= malloc(sizeof(mail));

    if(loginServerSide(argTh->con.ds_sock, packRecive) == -1){
        printf("Login fallito; disconnessione...\n");
        pthread_exit(NULL);
    }  /// RIVEDERE LA PARTE DELL'IF, RIMANGONO TROPPI ACCEPT NON LIBERATI COMPLETAMENTE IN QUESTO MODO

    argTh->arg = packRecive;

    printf("mi metto in ascolto\n");
    pthread_t tidRX, tidTX;

    pthread_create(&tidRX,NULL, thrServRX,argTh);
    pthread_create(&tidTX,NULL, thrServTX,argTh);

    pause();
    pthread_exit(NULL);
}

void *thrServRX(thConnArg *argTh){

    mail *packTest = argTh->arg;

    while(1) {
        sem_wait(&sem[0]);

        printf("Entro nel semaforo di andata\n");

        readPack(argTh->con.ds_sock, packTest);
        printf("Numero byte pacchetto: %d\n", packTest->md.dim);
        printf("Stringa da client: %s\n\n", packTest->mex);
        if (strcmp(packTest->mex, "quit") == 0) {
            break;
        }

        sem_post(&sem[1]);
        printf("Esco dal semaforo di ritorno\n");

        //writePack(); da aggiungere il selettore chat
    }
    close(argTh->con.ds_sock);
    free(argTh);
    pthread_exit(0);
}

void *thrServTX(thConnArg * argTh){

    mail *packTest = argTh->arg;

    while(1){
        //readPack() da aggiungere il selettore in ingresso di chat

        sem_wait(&sem[1]);
        printf("Entro nel semaforo di ritorno\n");

        writePack(argTh->con.ds_sock,packTest);
        if(errno){
            exit(-1);
        }


        sem_post(&sem[0]);
        printf("Esco dal semaforo di ritorno\n");
    }
}


int clientDemo(int argc, char *argv[]) {
    con = initSocket(atoi(argv[2]), argv[3]);

    if (initClient(con) == -1) {
        exit(-1);
    }

    mail *packSend = malloc(sizeof(mail));
    mail *packReceive = malloc(sizeof(mail));

    /*
    int attempts = 5;
    while(1) {
        if (!attempts){
            printf("Troppi tentativi effettuati; chiusura programma...\n");
            exit(-1);
        }
        if (loginUserSide(con->ds_sock, packSend) == -1) {
            attempts--;
            printf("Login fallito, tentativi rimasti: %d. Riprovare.\n",attempts);
            continue;
        }
        else break;
    }

    signal(SIGINT, helperClient);
    printf("Login Effettuato; per la lista di comandi, premere la combinazione CTRL+C\n");


    pthread_t tid;
    pthread_create(&tid, NULL, thrUserRX, packReceive);
    */
    do
    {
        free(packSend->mex);
        antiSegFault:
        printf("\nInserire messaggio:");
        scanf("%m[^\n]", &packSend->mex);
        fflush(stdin);

        if (packSend->mex == NULL) goto antiSegFault;

        //fillPack(packSend,1,"Utente_generico","testing_code",packsend->mex,strlen(packSend->mex)+1);

        packSend->md.dim=strlen(packSend->mex)+1;
        packSend->md.type=1;
        strncpy(packSend->md.sender,"Utente_generico",28);
        strncpy(packSend->md.whoOrWhy,"testing_code",24);
        //printf("Stringa inserita: %s\n", packSend->mex);
        writePack(con->ds_sock,packSend);
        printPack(packSend);

        //sleep(1);

        printf("\n\nRitorno\n");

        readPack(con->ds_sock, packReceive);
        printPack(packReceive);

    }
    while(strcmp(packSend->mex,"quit") != 0);
    free(packSend->mex);
    free(packSend);
    close(con->ds_sock);
    return 0;
}

void* thrUserRX(mail *pack){

}

void *helperClient(int sig){
    printf("Lista comandi per Client:\n\n");
    // AGGIUNGERE TUTTE LE POSSIBILITA' DA CLIENT
}

void helpProject(void)
{
    printf("I parametri inseribili sono:\n");
    printf("\tServer command:\n");
    printf("\ts [port] [coda]\t\tentro in modalita' server alla porta specificata\n");
    printf("\tClient command:\n");
    printf("\tc [port] [IP]\t\tentro in modalita' client alla porta e IP specificati\n");
}