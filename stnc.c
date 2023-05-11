#include "headers.h"

int main(int argc, char *argv[])
{
    printf("heloo 0");

    if (argc < 2)
    {

        printf("For part A : \n\t Usage: ./stnc -c [IP] [PORT]  \n\t\t or \n\t Usage: ./stnc -s [PORT]\n");
        printf("For part B : \n\t Usage: ./stnc -c [IP] [PORT] -p [type] [param] \n\t\t or \n\t Usage: ./stnc -s [PORT] -p -q\n");
        return -1;
    }

    printf("heloo 0");

    int client = 0;
    int server = 0;
    int performance = 0;
    int quiet = 0;
    int serverPort = 0;
    char *serverIp;
    char *type;
    char *param;

    printf("heloo 1");
    for (int i = 1; i < argc; i++)
    {
        printf("heloo 2");
        char *token = argv[i];
        if (!strcmp(token, "-c"))
        {
            printf("heloo 3");
            if (argc <= i + 2 || server)
            {

                return -1;
            }
            printf("heloo 4");
            client = 1;
            serverIp = argv[++i];
            serverPort = atoi(argv[++i]);
            continue;
        }
        printf("heloo 5");
        if (!strcmp(token, "-s"))
        {
            printf("heloo 6");
            if (argc <= i + 1 || client)
            {
                printf("usage: ./stnc -s [PORT] -p [type] [param] -q\n");
                return -1;
            }
            printf("heloo 7");
            server = 1;
            serverPort = atoi(argv[++i]);
            continue;
        }
        if (!strcmp(token, "-p"))
        {
            performance = 1;
            if (client)
            {
                if (argc <= i + 2)
                {
                    printf("usage: ./stnc -c [IP] [PORT] -p [type] [param]\n");
                    return -1;
                }
                type = argv[++i];
                param = argv[++i];
            }
            continue;
        }
        if (!strcmp(token, "-q"))
        {

            if (client)
            {
                printf("usage: ./stnc -c [IP] [PORT] -p [type] [param]\n");
                return -1;
            }
            quiet = 1;
            continue;
        }
    }

    printf("heloo 8");
    if (client)
    {
        printf("heloo 9");
        if (performance && !quiet)
        {
            printf("heloo 10");
            clientB(serverIp, serverPort, type, param);
        }

        else
            clientA(serverIp, serverPort);
    }
    else if (server)
    {
        if (performance)
            serverB(serverPort, quiet);
        else
            serverA(serverPort);
    }
    else
    {
    }

    return 0;
}
