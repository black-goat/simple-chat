#include <windows.h>
#include <winsock2.h>
#include <stdio.h>

#define NETWORK_ERROR -1
#define NETWORK_OK     0

void ReportError( int errorCode, const char* whichFunc );

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmd, int nShow )
{
    WORD sockVersion;
    WSADATA wsaData;
    int nret;

    sockVersion = MAKEWORD(2, 0);

    WSAStartup( sockVersion, &wsaData );

    SOCKET listeningSocket;

    listeningSocket = socket(   AF_INET,
                                SOCK_STREAM,
                                IPPROTO_TCP
                             );

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
    serverInfo.sin_port = htons(8888);

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

    closesocket( theClient );
    closesocket( listeningSocket );

    WSACleanup();
    return NETWORK_OK;
}

void ReportError( int errorCode, const char* whichFunc )
{
    char errorMsg[ 128 ];

    ZeroMemory( errorMsg, 128 );

    sprintf( errorMsg, "Call to %s returned error %d\n", (char *)whichFunc, errorCode );

    MessageBox( NULL, errorMsg, NULL, MB_OK );
}
