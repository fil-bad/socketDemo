#include <stdio.h>
#include <string.h>
#include "socketConnect.h"


void helpProject(void);
int serverDemo();
int clientDemo();



connection *con;

int main(int argc, char *argv[])
{

    if (argc >= 3)
    {
        if(strcmp(argv[1], "s") == 0)
        {
            con = initSocket(atoi(argv[2]),"INADDR_ANY");
            serverDemo();
            return 0;
        }
    }
    if (argc >= 4)
    {
        if(strcmp(argv[1], "c") == 0)
        {
            con = initSocket(atoi(argv[2]),argv[3]);
            clientDemo();
            return 0;
        }
    }
    helpProject();

    return 0;
}

int serverDemo()
{
    return 0;
}

int clientDemo()
{
    return 0;
}

void helpProject(void)
{
    printf("I parametri inseribili sono:\n");
    printf("\tServer command:\n");
    printf("\ts [port]\t\tentro in modalita' server alla porta specificata\n");
    printf("\tClient command:\n");
    printf("\tc [port] [IP]\t\tentro in modalita' client alla porta e IP specificati\n");
}