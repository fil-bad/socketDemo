#include <stdio.h>
#include <string.h>

#include "socketConnect.h"


void helpProject(void);
int serverDemo(int argc, char *argv[]);
int clientDemo(int argc, char *argv[]);



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
    while(1){
        if(acceptCreate(con,threadUser, NULL) == -1){
            exit(-1);
        }
    }

    return 0;
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