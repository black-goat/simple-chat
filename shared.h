// Shared constants and functions between client.c and server.c

#ifndef SHARED_H_INCLUDED
#define SHARED_H_INCLUDED

#include <windows.h>

#define NETWORK_ERROR -1
#define NETWORK_OK     0

#define HOST_BY_NAME   0
#define HOST_BY_ADDR   1

#define MODE_SEND   0
#define MODE_RECV   1

extern const char* QUIT_STRING;

void init_winsock( WORD version );
void ReportError( int errorCode, const char* whichFunc );

// Starts messaging loop, mode can be either MODE_SEND for sending date, or MODE_RECV for receiving
void start_message_loop( SOCKET theSocket, int mode );

// Reads line from socket using recv()
char* readLine( SOCKET s );

int is_exit_string( const char* str );
int is_quit_string( const char* str );

#endif // SHARED_H_INCLUDED
