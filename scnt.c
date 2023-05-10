#include "headers.h"

int main(int argc, char *argv[])
{
    int client = 0;
    int server = 0;
    int performance = 0;
    int quiet = 0;
    int serverPort = 0;
    char *serverIp;
    char *type;
    char *param;
    
    for (int i = 1; i < argc; i++)
    {
        char* token = argv[i];

        if (!strcmp(token, "-c"))
        {
            if (argc <= i + 2 || server)
            {
                printf("usage: ./stnc -c [IP] [PORT] -p [type] [param]\n");
                return -1;
            }
            client = 1;
            serverIp = argv[++i];
            serverPort = htons(argv[++i]);
            continue;
        }
        if (!strcmp(token, "-s"))
        {
            if (argc <= i + 1 || client)
            {
                printf("usage: ./stnc -s [PORT] -p [type] [param] -q\n");
                return -1;
            }
            server = 1;
            serverPort = htons(argv[++i]);
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
                printf("usage: ./stnc -c [IP] [PORT] -p [type] [param] \n\t\tor\n\t./stnc -s [PORT] -p -q\n");
                return -1;
            }
            quiet = 1;
            continue;
        }
    }
    
    if (quiet && !performance)
        printf("usage: ./stnc -s [PORT] -p -q\n");

    if (client)
    {
        if (performance)
            clientB(serverIp, serverPort, type, param);
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
        printf("usage:  ./stnc -c [IP] [PORT] -p [type] [param] -q\n\t\tor\n\t./stnc -s [PORT] -p [type] [param] -q\n");
        return -1;
    }

    return 0;
}
