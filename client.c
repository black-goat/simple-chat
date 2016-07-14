#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include "shared.h"
#include "client.h"

int client()
{
    int nret; // Used for error reporting

    char hostAddress[128];
    unsigned int hostPort;

    printf("Enter the address of the host you'd like to connect to: ");
    scanf("%s", hostAddress);
    printf("Enter the port you'd like to make a connection on[0-65535]: ");
    do{
        scanf("%d", &hostPort );
        if( hostPort > 65535 )
            printf("Port out of bounds! Please enter a number in the range [0-65535]: ");
    }while( hostPort > 65535 );

    LPHOSTENT hostInfo;
    hostInfo = gethostbyname(hostAddress);

    //IN_ADDR iaHost;
    //iaHost.s_addr = inet_addr(hostAddress);
    //hostInfo = gethostbyaddr( (const char*)&iaHost.s_addr, sizeof( struct in_addr ), AF_INET );

    if (!hostInfo)
	{
		nret = WSAGetLastError();
		ReportError(nret, "gethostbyname()");

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

		return NETWORK_ERROR;
	}

    SOCKADDR_IN serverInfo;

    serverInfo.sin_family = AF_INET;

    serverInfo.sin_addr = *( (LPIN_ADDR)hostInfo->h_addr_list[0] );

    serverInfo.sin_port = htons( hostPort );

    nret = connect(     theSocket,
                        (LPSOCKADDR)&serverInfo,
                        sizeof( struct sockaddr ) );

    if ( nret == SOCKET_ERROR )
	{
		nret = WSAGetLastError();
		ReportError(nret, "connect()");

		return NETWORK_ERROR;
	}

    char* message = readLine( theSocket );
    printf( "%s\n", message );

    start_message_loop( theSocket, MODE_SEND );

	closesocket( theSocket );

	system( "PAUSE" );

	return NETWORK_OK;
}
