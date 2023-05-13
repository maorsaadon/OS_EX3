#include "headers.h"

void pr(int option)
{

    switch (option)
    {
    case 0:
        printf("For part A : \n\t Usage: ./stnc -c [IP] [PORT]  \n\t\t or \n\t Usage: ./stnc -s [PORT]\n");
        printf("For part B : \n\t Usage: ./stnc -c [IP] [PORT] -p [type] [param]  \n\t\t or \n\t Usage: ./stnc -s [PORT] -p -q\n");
        break;
    case 1:
        printf("For part A : \n\t Usage: ./stnc -c [IP] [PORT]  \n or \n For part B : \n\t Usage: ./stnc -c [IP] [PORT] -p [type] [param]\n");
        break;
    case 2:
        printf("For part A : \n\t Usage: ./stnc -s [PORT]  \n or \n For part B : Usage: ./stnc -s [PORT] -p -q\n");
        break;
    case 3:
        printf("Usage: ./stnc -c [IP] [PORT]  \n");
    case 4:
        printf("Usage: ./stnc -s [PORT]  \n");
    case 5:
        printf("Usage: ./stnc -c [IP] [PORT] -p [type] [param]  \n");
    case 6:
        printf("Usage: ./stnc -s [PORT] -p -q  \n");
    default:
        break;
    }

    exit(-1);
}

int main(int argc, char *argv[])
{

    if (argc < 3 || argc > 7)
        pr(0);
    
    int q = 0, p = 0, serverPort = 0, c = 0, s = 0;
    char *serverIp, *type, *param;

    for (int i = 1; i < argc; i++)
    {
        char* token = argv[i];
        if (!strcmp(token , "-c"))
        {
            if (argc <= i + 2 || s)
                pr(5);
            c = 1;
            serverIp = argv[++i];
            serverPort = atoi(argv[++i]);
            continue;
        }
        if (!strcmp(token, "-s"))
        {
            if (argc <= i + 1 || c)
                pr(6);
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
                    pr(5);
                type = argv[++i];
                param = argv[++i];
            }
            continue;
        }
        if (!strcmp(argv[i], "-q"))
        {

            if (c)
                pr(1);
            q = 1;
            continue;
        }
    }
    if (q && !p)
        pr(2);

    
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
    else
        pr(0);
    return 0;
}
