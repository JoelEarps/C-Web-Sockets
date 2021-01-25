
#include <wS2tcpip.h>
#include <iostream>
#include <string>
// winstock is th API windows uses to access sockets, WS2tcpip file incudes most of the header files
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	//intialise winstock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2); //give version 2.2

	int wsOk = WSAStartup(ver, &wsData); //version of Winstock and pointer to data
	if (wsOk != 0) {
		cerr << "Can't Initialise winstock! Quitting" << endl; //if can't intialise winstock leave program early
		return;
	}

	// create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "Can't create a socket! Quitting" << endl; //if can't intialise winstock leave program early
		return;
	}
	//bind the socket to an ip address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000); //host to network short
	hint.sin_addr.S_un.S_addr = INADDR_ANY; //could use inet_pton..

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//tell the socket to listen - tell winstock socket is for listening
	listen(listening, SOMAXCONN);//just marks it for listening - doesnt actually accept any connections
	//SOMAXCONN - is number of max connections it can manage 

	//wait for connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize); //socket that retunrs this value is someother random number

	char host[NI_MAXHOST]; //clients remote name
	char service[NI_MAXHOST];//service (port) client is connected on

	ZeroMemory(host, NI_MAXHOST); //zeroMemory is a defined type by windows
	ZeroMemory(service, NI_MAXHOST);

	//check to see if we can get name info - if can't we rely on info we can get
	//look up for host name if not display IP address

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << "Host connected on port" << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_NUMERICHOST);
		cout << "connected on port" <<
			ntohs(client.sin_port);

	}

	//close the listening socket
	closesocket(listening);
	//while loop and and echo message back to client
	char buf[4096];
	while (true) {
		ZeroMemory(buf, 4096);
		//wait for client to send data - use recieve function returns number of bytes received - if get an error we aren't going to do anything
		//bytes recieved 0 means cleint disconnnected
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error in recv(). Quitting!" << endl;
		}
		if (bytesReceived == 0) {
			cerr << "Client disconnected" << endl;
		}
		//echo message back to client
		cout << string(buf, 0, bytesReceived) << endl;
		send(clientSocket, buf, bytesReceived + 1, 0); //when recieve a message we do not get terminating 0, +1 auto sends the 0 out		
	}
	//close the socket
	closesocket(clientSocket);
	//shutdown winstock
	WSACleanup();
}