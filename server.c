#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include "shared.h"
#include "server.h"

int server()
{
    int nret; // Used for error reporting

    unsigned int listeningPort;
    printf("Enter the port you would like to listen on[0-65535]: ");
    do{
        scanf("%d", &listeningPort );
        if( listeningPort > 65535 )
            printf("Port out of bounds! Please enter a number in the range [0-65535]: ");
    }while( listeningPort > 65535 );

    SOCKET listeningSocket;

    listeningSocket = socket(   AF_INET,
                                SOCK_STREAM,
                                IPPROTO_TCP );

    if( listeningSocket == INVALID_SOCKET )
    {
        nret = WSAGetLastError();
        ReportError( nret, "socket()" );

        WSACleanup();
        return NETWORK_ERROR;
    }

    SOCKADDR_IN serverInfo;

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons( listeningPort );

    nret = bind( listeningSocket, (LPSOCKADDR)&serverInfo, sizeof(struct sockaddr) );

    if( nret == SOCKET_ERROR )
    {
        nret = WSAGetLastError();
        ReportError( nret, "bind()" );

        WSACleanup();
        return NETWORK_ERROR;
    }

    nret = listen( listeningSocket, 1 );

    if( nret == SOCKET_ERROR )
    {
        nret = WSAGetLastError();
        ReportError( nret, "listen()" );

        WSACleanup();
        return NETWORK_ERROR;
    }

    SOCKET theClient;

    printf( "Awaiting connections...\n" );
    theClient = accept(     listeningSocket,
                            NULL,
                            NULL );
    if( theClient == INVALID_SOCKET )
    {
        nret = WSAGetLastError();
        ReportError( nret, "accept()" );

        WSACleanup();
        return NETWORK_ERROR;
    }
    printf( "Client has connected!\n" );

    char welcomeMessage[] = "Welcome to the server!\n";
    send( theClient, welcomeMessage, strlen( welcomeMessage ), 0 );

    start_message_loop( theClient, MODE_RECV );

    closesocket( theClient );
    closesocket( listeningSocket );

    system( "PAUSE" );

    return NETWORK_OK;
}
