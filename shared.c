#include "shared.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

const char* QUIT_STRING = "#quit";

void init_winsock( WORD version )
{
    WSADATA wsaData;

    WSAStartup( version, &wsaData );

    return;
}

void ReportError( int errorCode, const char* whichFunc )
{
    char errorMsg[ 128 ];

    ZeroMemory( errorMsg, 128 );

    sprintf( errorMsg, "Call to %s returned error %d", (char *)whichFunc, errorCode );

    MessageBox( NULL, errorMsg, NULL, MB_OK );
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

void start_message_loop( SOCKET theSocket, int mode )
{
    fflush( stdin );

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
            buffer[--pos] = '\0';
            send( theSocket, buffer, strlen( buffer ) + 1, 0 );
            if( is_exit_string( buffer ) )
            {
                mode = MODE_RECV;
            }
            if( is_quit_string( buffer ) )
            {
                printf("You have disconnected!\n");
                break;

            }
        }
        else
        {
            char* theMessage;
            theMessage = readLine( theSocket );
            if( theMessage == NULL )
            {
                printf("Could not retrieve message! Other has probably disconnected.\n");
                break;
            }
            if( is_exit_string( theMessage ) )
            {
                mode = MODE_SEND;
            }
            if( is_quit_string( theMessage ) )
            {
                printf( "Other has disconnected!\n" );
                break;
            }

            printf( "Other: %s\n", theMessage );
        }
    }

    printf( "Thanks for coming!\n" );
}

int is_quit_string( const char* str )
{
    if( strcmp( QUIT_STRING, str ) == 0 )
        return 1;
    else
        return 0;

}

int is_exit_string( const char* str )
{
    if( str[0] == '$' && (str[1] == '\n' || str[1] == NULL ) )
        return 1;
    else
        return 0;
}
