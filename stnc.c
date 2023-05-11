#include "headers.h"

void pr(char *type)
{

    if (!strcmp(type, "clientA"))
        printf("Usage: ./stnc -c [IP] [PORT]  \n");
    if (!strcmp(type, "serverA"))
        printf("Usage: ./stnc -s [PORT]  \n");
    if (!strcmp(type, "clientB"))
        printf("Usage: ./stnc -c [IP] [PORT] -p [type] [param]  \n");
    if (!strcmp(type, "serverB"))
        printf("Usage: ./stnc -s [PORT] -p -q  \n");

    exit(-1);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {

        printf("For part A : \n\t Usage: ./stnc -c [IP] [PORT]  \n\t\t or \n\t Usage: ./stnc -s [PORT]\n");
        printf("For part B : \n\t Usage: ./stnc -c [IP] [PORT] -p [type] [param]  \n\t\t or \n\t Usage: ./stnc -s [PORT] -p -q\n");
        return -1;
    }

    int c = 0, s = 0, p = 0, q = 0, serverPort = 0;
    char *serverIp, type, param;

    for (int i = 1; i < argc; i++)
    {
        char *token = argv[i];
        if (!strcmp(token, "-c"))
        {
            if (argc <= i + 2 || s)
                pr("clientB");

            c = 1;
            serverIp = argv[++i];
            serverPort = atoi(argv[++i]);
            continue;
        }
        if (!strcmp(token, "-s"))
        {
            if (argc <= i + 1 || c)
                pr("serverB");
            s = 1;
            serverPort = atoi(argv[++i]);
            continue;
        }
        if (!strcmp(token, "-p"))
        {
            p = 1;
            if (c)
            {
                if (argc <= i + 2)
                    pr("clientB");
                type = argv[++i];
                param = argv[++i];
            }
            continue;
        }
        if (!strcmp(token, "-q"))
        {
            if (c)
                pr("clientB");
            q = 1;
            continue;
        }
    }

    if (c)
    {
        if (p && !q)
            clientB(serverIp, serverPort, type, param);
        else
            clientA(serverIp, serverPort);
    }
    else if (s)
    {
        if (p)
            serverB(serverPort, q);
        else
            serverA(serverPort);
    }

    return 0;
}
