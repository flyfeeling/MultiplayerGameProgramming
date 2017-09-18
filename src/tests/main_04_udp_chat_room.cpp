#include "Multiplayer.h"
#include <iostream>
#include <string>

const int MTU = 1300;

#define CMD_CONNECT "cmd:connect"
#define CMD_DISCONNECT "cmd:disconnect"

void server(const char *serverPortStr)
{
	// Create and configure the server socket
	UDPSocketPtr serverSocket = SocketUtil::CreateUDPSocket(INET);

	//serverSocket->SetNonBlockingMode(true);

	auto res = serverSocket->SetReuseAddress(true);
	if (res != NO_ERROR) { return; }

	std::string serverAddress = std::string("127.0.0.1:") + serverPortStr;
	SocketAddress selfAddress(serverAddress.c_str());
	res = serverSocket->Bind(selfAddress);
	if (res != NO_ERROR) { return; }

	// A client address
	SocketAddress clientAddress;

	// Buffers
	char inBuffer[MTU];
	std::string inString;

	std::set<SocketAddress> clientAddresses;

	while (1)
	{
		// Receive information
		auto recvBytes = serverSocket->ReceiveFrom(inBuffer, MTU, clientAddress);

		if (recvBytes > 0) {

			inString = inBuffer;
			std::cout << inString << std::endl;

			if (inString == CMD_CONNECT)
			{
				clientAddresses.insert(clientAddress);
			}
			else if (inString == CMD_DISCONNECT)
			{
				clientAddresses.erase(clientAddress);
			}
			else
			{
				bool found = false; // to check new clients

				for (auto address : clientAddresses) {
					if (!(address == clientAddress)) {
						auto sentBytes = serverSocket->SendTo(inBuffer, recvBytes, address);
					}
				}

				if (!found) {
					clientAddresses.insert(clientAddress);
				}
			}
		}
	}
}

void client(const char *selfAddressStr, const char *serverAddressStr)
{
	// Create and configure the client socket
	UDPSocketPtr clientSocket = SocketUtil::CreateUDPSocket(INET);

	clientSocket->SetNonBlockingMode(true);

	auto res = clientSocket->SetReuseAddress(true);
	if (res != NO_ERROR) { return; }

	SocketAddress selfAddress(selfAddressStr);
	res = clientSocket->Bind(selfAddress);
	if (res != NO_ERROR) { return; }

	// Create the server address
	SocketAddress fromAddress;
	SocketAddress serverAddress(serverAddressStr);

	// Send the connect command
	auto sentBytes = clientSocket->SendTo(CMD_CONNECT, strlen(CMD_CONNECT)+1, serverAddress);

	char inBuffer[MTU];
	char outBuffer[MTU];
	std::string opt;

	bool running = true;

	while (running)
	{
		std::cout << std::endl;
		std::cout << "Menu" << std::endl;
		std::cout << "1) Send a message" << std::endl;
		std::cout << "2) Receive messages" << std::endl;
		std::cout << "3) Exit" << std::endl;
		std::cout << "Choose an option: " << std::flush;

		std::cin >> opt; std::cin.ignore();

		if (opt == "1")
		{
			// Send information
			std::cout << "Type the message: " << std::flush;
			std::cin.getline(outBuffer,sizeof(outBuffer));
			auto outLen = strlen(outBuffer);
			if (outLen > 0) {
				auto sentBytes = clientSocket->SendTo(outBuffer, outLen+1, serverAddress);
			}
		}
		else if (opt == "2")
		{
			// Receive information
			auto recvBytes = clientSocket->ReceiveFrom(inBuffer, MTU, fromAddress);
			if (recvBytes > 0) {
				std::cout << inBuffer <<std::endl;
			}
		}
		else if (opt == "3")
		{
			// Send disconnect message
			auto sentBytes = clientSocket->SendTo(CMD_DISCONNECT, strlen(CMD_DISCONNECT)+1, serverAddress);
			running = false;
		}
		else
		{
			std::cout << "Unknown option" << std::endl;
		}

	}
}

void printUsage(int argc, const char **argv)
{
	std::cout << "For server mode: " << argv[0] << " server <server_port>" << std::endl;
	std::cout << "For client mode: " << argv[0] << " client <self_ip_address>:<port> <server_ip_address>:<port>" << std::endl;
}

int myMain(int argc, const char **argv)
{
	if (argc == 3) {
		std::string mode = argv[1];
		if (mode != "server") {
			printUsage(argc, argv);
			return -1;
		}
		const char *serverPortStr = argv[2];
		server(serverPortStr);
	} else if (argc == 4) {
		std::string mode = argv[1];
		if (mode != "client") {
			printUsage(argc, argv);
			return -1;
		}
		const char *clientAddressStr = argv[2];
		const char *serverAddressStr = argv[3];
		client(clientAddressStr, serverAddressStr);
	} else {
		printUsage(argc, argv);
		return -1;
	}

	return 0;
}

#if _WIN32
int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	return myMain(__argc, __argv);
}
#else
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;

	return myMain(argc, argv);
}
#endif

