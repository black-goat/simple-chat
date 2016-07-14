#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include "shared.h"
#include "server.h"
#include "client.h"

#define BE_SERVER   1
#define BE_CLIENT   2

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmd, int nShow )
{
    init_winsock( MAKEWORD(2, 0) );

    printf("Welcome to simple chat!\n");
    printf("Please select if you wish to act as a server, or as a client: \n");
    printf("1 - Server\n");
    printf("2 - Client\n");

    int select;
    do{
        scanf("%d", &select);
        if( select != BE_SERVER && select != BE_CLIENT )
            printf("%d is not a valid option! Please select either 1 to be a server or 2 to be a client.\n");
    }while( select != BE_SERVER && select != BE_CLIENT );

    if( select == BE_SERVER )
    {
        printf("You have selected to be a server!\n");
        server();
    }
    if( select == BE_CLIENT )
    {
        printf("You have selected to be a client!\n");
        client();
    }

    return NETWORK_OK;
}
