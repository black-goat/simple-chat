#include <windows.h>
#include <winsock2.h>
#include <stdio.h>

#define NETWORK_ERROR -1
#define NETWORK_OK     0

#define HOST_BY_NAME   0
#define HOST_BY_ADDR   1

#define MODE_SEND   0
#define MODE_RECV   1

void ReportError( int errorCode, const char* whichFunc );

char* readLine( SOCKET s );
int is_exit_string( char* str );

void init_winsock( WORD version );
int get_host_info( LPHOSTENT hostInfo, int getHostBy, const char* host )
{
    if( getHostBy == HOST_BY_NAME )
    {
        hostInfo = gethostbyname( host );
    }
    else if( getHostBy == HOST_BY_ADDR )
    {
        IN_ADDR iaHost;
        iaHost.s_addr = inet_addr( host );
        hostInfo = gethostbyaddr( (const char*)iaHost.s_addr, sizeof( struct in_addr ), AF_INET );
    }
    else
    {
        MessageBox( NULL, "Error: No such command %d", "get_host_info() failed", 0 );
    }

    printf( "%d\n", (hostInfo == NULL ) );

    if( !hostInfo )
    {
        int nret = WSAGetLastError();
        switch( getHostBy )
        {
        case HOST_BY_NAME:
            ReportError( nret, "gethostbyname()" );
            break;
        case HOST_BY_ADDR:
            ReportError( nret, "gethostbyaddr()" );
            break;
        default:
            MessageBox( NULL, "Invalid operation requested", "get_host_by() failed", 0 );
            break;
        }

        return -1;
    }

    return 0;
}

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmd, int nShow )
{
    int nret; // Used for error reporting

    init_winsock( MAKEWORD( 2, 0 ) );

    LPHOSTENT hostInfo;
    IN_ADDR iaHost;

    iaHost.s_addr = inet_addr("192.168.100.5");

    hostInfo = gethostbyname("192.168.100.5");
    //hostInfo = gethostbyaddr( (const char*)&iaHost.s_addr, sizeof( struct in_addr ), AF_INET );

    if (!hostInfo)
	{
		nret = WSAGetLastError();
		ReportError(nret, "gethostbyname()");

		WSACleanup();
		return NETWORK_ERROR;
	}

    SOCKET theSocket;

    theSocket = socket(     AF_INET,
                            SOCK_STREAM,
                            IPPROTO_TCP );

    if ( theSocket == INVALID_SOCKET )
	{
		nret = WSAGetLastError();
		ReportError(nret, "socket()");

		WSACleanup();
		return NETWORK_ERROR;
	}

    SOCKADDR_IN serverInfo;

    serverInfo.sin_family = AF_INET;

    serverInfo.sin_addr = *( (LPIN_ADDR)hostInfo->h_addr_list[0] );

    serverInfo.sin_port = htons( 8888 );

    nret = connect(     theSocket,
                        (LPSOCKADDR)&serverInfo,
                        sizeof( struct sockaddr ) );

    if ( nret == SOCKET_ERROR )
	{
		nret = WSAGetLastError();
		ReportError(nret, "connect()");

		WSACleanup();
		return NETWORK_ERROR;
	}

    char* message = readLine( theSocket );
    printf( "%s\n", message );

    int mode = MODE_SEND;

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
            send( theSocket, buffer, strlen( buffer ), 0 );
            if( is_exit_string( buffer ) )
            {
                mode = MODE_RECV;
            }
        }
        else
        {
            char* theMessage;
            theMessage = readLine( theSocket );
            if( is_exit_string( theMessage ) )
            {
                mode = MODE_SEND;
            }

            printf( "Other: %s\n", theMessage );
        }
    }

    system( "PAUSE" );

	closesocket( theSocket );
	WSACleanup();
	return NETWORK_OK;
}

int is_exit_string( char* str )
{
    if( str[0] == '$' && (str[1] == '\n' || str[1] == NULL ) )
        return 1;
    else
        return 0;
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

void ReportError( int errorCode, const char* whichFunc )
{
    char errorMsg[ 128 ];

    ZeroMemory( errorMsg, 128 );

    sprintf( errorMsg, "Call to %s returned error %d", (char *)whichFunc, errorCode );

    MessageBox( NULL, errorMsg, NULL, MB_OK );
}

void init_winsock( WORD version )
{
    WSADATA wsaData;

    WSAStartup( version, &wsaData );

    return;
}
