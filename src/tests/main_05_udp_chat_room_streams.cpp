#include "Multiplayer.h"
#include <iostream>
#include <string>

enum MessageType {
	MT_CLIENT_CONNECT,
	MT_CLIENT_DISCONNECT,
	MT_CLIENT_TEXT,
	MT_CLIENT_LAST
};

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
	InputMemoryStream inputStream;

	std::set<SocketAddress> clientAddresses;

	while (1)
	{
		// Receive information
		inputStream.Clear();
		auto recvBytes = serverSocket->ReceiveFrom(inputStream.GetBufferPtr(), inputStream.GetCapacity(), clientAddress);

		if (recvBytes > 0) {

			int command;
			inputStream.Read(command);
			std::cout << inputStream.GetBufferPtr() << std::endl;

			if (command == MT_CLIENT_CONNECT)
			{
				clientAddresses.insert(clientAddress);
			}
			else if (command == MT_CLIENT_DISCONNECT)
			{
				clientAddresses.erase(clientAddress);
			}
			else
			{
				bool found = false; // to check new clients

				// Forward the message
				for (auto address : clientAddresses) {
					if (!(address == clientAddress)) {
						auto sentBytes = serverSocket->SendTo(inputStream.GetBufferPtr(), recvBytes, address);
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

	// Create input and output streams
	InputMemoryStream inputStream;
	OutputMemoryStream outputStream;

	// Send the connect command
	outputStream.Write(MT_CLIENT_CONNECT);
	auto sentBytes = clientSocket->SendTo(outputStream.GetBufferPtr(), outputStream.GetSize(), serverAddress);

	std::string opt;
	char textline[512];
	bool running = true;
	std::string clientName("Client");
	std::string remoteClientName;
	std::string remoteTextLine;

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
			std::cin.getline(textline,sizeof(textline));
			auto outLen = strlen(textline);
			if (outLen > 0) {
				outputStream.Clear();
				outputStream.Write(MT_CLIENT_TEXT);
				outputStream.Write(clientName);
				outputStream.Write(std::string(textline));
				auto sentBytes = clientSocket->SendTo(outputStream.GetBufferPtr(), outputStream.GetSize(), serverAddress);
			}
		}
		else if (opt == "2")
		{
			// Receive information
			inputStream.Clear();
			auto recvBytes = clientSocket->ReceiveFrom(inputStream.GetBufferPtr(), inputStream.GetCapacity(), fromAddress);

			std::cout << std::endl;
			while (recvBytes > 0) {
				// Read the packet
				int command;
				std::string name, textline;
				inputStream.Read(command);
				inputStream.Read(remoteClientName);
				inputStream.Read(remoteTextLine);
				std::cout << remoteClientName << ": " << remoteTextLine <<std::endl;

				// Receive information
				inputStream.Clear();
				recvBytes = clientSocket->ReceiveFrom(inputStream.GetBufferPtr(), inputStream.GetCapacity(), fromAddress);
			}
		}
		else if (opt == "3")
		{
			// Send disconnect message
			outputStream.Clear();
			outputStream.Write(MT_CLIENT_DISCONNECT);
			auto sentBytes = clientSocket->SendTo(outputStream.GetBufferPtr(), outputStream.GetSize(), serverAddress);
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

