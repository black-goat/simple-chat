#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#define NETWORK_ERROR -1
#define NETWORK_OK     0

void ReportError( int errorCode, const char* whichFunc );

char* readLine( SOCKET s );

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmd, int nShow )
{
    WORD sockVersion;
    WSADATA wsaData;
    int nret;

    sockVersion = MAKEWORD(1, 1);

    WSAStartup( sockVersion, &wsaData );

    LPHOSTENT hostInfo;
    IN_ADDR iaHost;

    iaHost.s_addr = inet_addr("46.228.47.114");

    hostInfo = gethostbyname("yahoo.com");
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

	closesocket( theSocket );
	return NETWORK_OK;
}

char* readLine( SOCKET s )
{
    char* theMessage = (char*) malloc( 256 );
    ZeroMemory( theMessage, 256 );
    char buffer;
    int bytesReceived;

    while( TRUE )
    {
        int pos = 0;

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
