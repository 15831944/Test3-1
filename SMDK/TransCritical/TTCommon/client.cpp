//
// Client.cpp



//
// Extremely simple, stream client example.
// Works in conjunction with Server.cpp.
//
// The program attempts to connect to the server and port
// specified on the command line. The Server program prints
// the needed information when it is started. Once connected,
// the program sends data to the server, waits for a response
// and then exits.
//
// Compile and link with wsock32.lib
//
// Pass the server name and port number on the command line. 
//
// Example: Client MyMachineName 2000
//
#include "stdafx.h"
#include <stdio.h>
#include <winsock.h>
#include "client.h"
///// Function prototype
void StreamClient(char *szServer, short nPort);

// Helper macro for displaying errors
#define PRINTERROR(s)	\
		fprintf(stderr,"\n%: %d\n", s, WSAGetLastError())


static SOCKET theSocket;
static int sockOpen = 0;




void OpenClient(char *szServer, short nPort)
{
  
  WORD wVersionRequested = MAKEWORD(1,1);
  WSADATA wsaData;
  LPHOSTENT lpHostEntry;
  int nRet;

  if (sockOpen) return;
  
  

  nRet = WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested)
    {	
      sockOpen = 0;
      fprintf(stderr,"\n Wrong version\n");
      WSACleanup();
      return;
    }

  lpHostEntry = gethostbyname(szServer);
  if (lpHostEntry == NULL)
    {
      sockOpen = 0;
      PRINTERROR("gethostbyname()");
      WSACleanup();
      return;
    }

  theSocket = socket(AF_INET,				// Address family
		     SOCK_STREAM,			// Socket type
		     IPPROTO_TCP);		// Protocol
  if (theSocket == INVALID_SOCKET)
    {
      sockOpen = 0;
      PRINTERROR("socket()");
      WSACleanup();
      return;
    }

  //
  // Fill in the address structure
  //
  SOCKADDR_IN saServer;

  saServer.sin_family = AF_INET;
  saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
  // ^ Server's address
  saServer.sin_port = htons(nPort);	// Port number from command line

  //
  // connect to the server

  //

  nRet = connect(theSocket,				// Socket
		 (LPSOCKADDR)&saServer,	// Server address
		 sizeof(struct sockaddr));// Length of server address structure
  if (nRet == SOCKET_ERROR)
    {
      sockOpen = 0;
      PRINTERROR("socket()");
      closesocket(theSocket);
      WSACleanup();
      return;
    }
  sockOpen = 1;
  

}


void SendToClient(char * msg) 
{
    char szBuf[256];

  strcpy(szBuf, msg);
  int nRet = send(theSocket,				// Connected socket
	      szBuf,					// Data buffer
	      strlen(szBuf),			// Length of data
	      0);						// Flags
  if (nRet == SOCKET_ERROR)
    {
      sockOpen = 0;
      PRINTERROR("send()");
      closesocket(theSocket);
      WSACleanup();
      return;
    }
  
}



void CloseClient() 
{
  if (sockOpen) {
    char msgstr[10];
    sprintf(msgstr, "Done");
    SendToClient(msgstr);
    sockOpen = 0;
    closesocket(theSocket);
  }
  return;
}





void CleanUp() 
{
  WSACleanup();
}

void NewLine() 
{
  char msgstr[10];
  sprintf(msgstr, "\n");
  SendToClient(msgstr);
}



bool bTTDebug = 1;

void p(char *s, double  x) {
  if (!bTTDebug) return;
  char msgstr[80];
  sprintf(msgstr, s, x);
  SendToClient(msgstr);
  NewLine();
}
void p(char *s, bool  x) {
  if (!bTTDebug) return;
  char msgstr[80];
  sprintf(msgstr, s, x);
  SendToClient(msgstr);
  NewLine();
}
void p(char *s, int  x) {
  if (!bTTDebug) return;
  char msgstr[80];
  sprintf(msgstr, s, x);
  SendToClient(msgstr);
  NewLine();
}
void p(char*s) 
{
  if (!bTTDebug) return;
  char msgstr[80];
  sprintf(msgstr, s);
  SendToClient(s);
  NewLine();
}
