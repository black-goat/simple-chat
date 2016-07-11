#include <windows.h>
#include <winsock2.h>
#include <stdio.h>

#define NETWORK_ERROR -1
#define NETWORK_OK     0

#define MODE_SEND   0
#define MODE_RECV   1

void ReportError( int errorCode, const char* whichFunc );

char* readLine( SOCKET s );
int is_exit_string( char* str );

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

    int mode = MODE_RECV;

    while( 1 )
    {
        if( mode == MODE_SEND )
        {
            printf( "You: " );
            int pos = 0;
            char buffer[ 256 ];
            ZeroMemory( buffer, 256 );
            char c;
            do{
                c = getchar();
                buffer[ pos ] = c;
                ++pos;
            }while( c != '\n' );
            send( theClient, buffer, strlen( buffer ), 0 );
            if( is_exit_string( buffer ) )
            {
                mode = MODE_RECV;
            }
        }
        else
        {
            char* theMessage;
            theMessage = readLine( theClient );
            if( is_exit_string( theMessage ) )
            {
                mode = MODE_SEND;
            }

            printf( "Other: %s\n", theMessage );
        }
    }

    closesocket( theClient );
    closesocket( listeningSocket );

    system( "PAUSE" );

    WSACleanup();
    return NETWORK_OK;
}

char* readLine( SOCKET s )
{
    char* theMessage = (char*) malloc( 256 );
    ZeroMemory( theMessage, 256 );
    char buffer;
    int bytesReceived;

    int pos = 0;

    while( TRUE )
    {
        bytesReceived = recv(   s,
                                &buffer,
                                1,
                                0 );

        if( bytesReceived <= 0 )
        {
            MessageBox( NULL, "recv() returned nothing.", NULL, MB_OK );
            return NULL;
        }

        if( buffer != '\0' && buffer != '\n' )
        {
            theMessage[ pos ] = buffer;
            ++pos;
        }
        else
        {
            return theMessage;
        }
    }
}

int is_exit_string( char* str )
{
    if( str[0] == '$' && (str[1] == '\n' || str[1] == NULL ) )
        return 1;
    else
        return 0;
}

void ReportError( int errorCode, const char* whichFunc )
{
    char errorMsg[ 128 ];

    ZeroMemory( errorMsg, 128 );

    sprintf( errorMsg, "Call to %s returned error %d\n", (char *)whichFunc, errorCode );

    MessageBox( NULL, errorMsg, NULL, MB_OK );
}
