#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	int port = 54000; // Server's Port

	// Initialise WINSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);

	int wsOK = WSAStartup(ver, &data);
	if (wsOK != 0) {
		std::cerr << "An error occured when trying to start WinSock, Err #" << wsOK << std::endl;
		return -1;
	}

	// Create a Socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		std::cerr << "An error occured when trying to create a Socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return -1;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);
	std::cout << "Listening Socket Established. Max Clients: " << SOMAXCONN << std::endl;
	std::cout << "Clients can connect on port: " << port << std::endl;

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " <<
			ntohs(client.sin_port) << std::endl;
	}

	// Close listening socket
	closesocket(listening);

	// While loop: accept and echo message back to client
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);

		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "An error occured in the receive function, closing server..." << std::endl;
			break;
		}

		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected " << std::endl;
			break;
		}

		// Echo message in console
		std::cout << host << "> " << std::string(buf, 0, bytesReceived) << std::endl;

		// Echo message back to client
		send(clientSocket, buf, bytesReceived + 1, 0);
	}

	// Cleanup Everything
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}
