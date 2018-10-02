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
void *threadUser(thConnArg *);




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
    con = initSocket(atoi(argv[2]),"INADDR_ANY");

    if(initServer(con,atoi(argv[3])) == -1){
        exit(-1);
    }
    pthread_t tid;
    thUserServ *arg;
    int i = 0;
    while(1){
        arg = malloc(sizeof(thUserServ));
        arg->id = i;
        if(acceptCreate(con,threadUser, arg) == -1) {
            exit(-1);
        }
        i++;
    }

    return 0;
}

void *threadUser(thConnArg * argTh){
    thUserServ *info = argTh->arg;
    printf("Id = %d\n", info->id);
    mail *pack = malloc(sizeof(mail));

    loginServerSide(argTh->con.ds_sock, pack);
    free(info);
    free(argTh);
}

int clientDemo(int argc, char *argv[])
{
    con = initSocket(atoi(argv[2]),argv[3]);

    if(initClient(con) == -1){
        exit(-1);
    }
    mail *pack = malloc(sizeof(mail));

    if(loginUserSide(con->ds_sock,pack) == -1){
        exit(-1);
    }

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