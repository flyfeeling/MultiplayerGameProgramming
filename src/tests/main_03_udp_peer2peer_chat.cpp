#include "Multiplayer.h"
#include <iostream>
#include <string>

const int MTU = 1300;

void mainLoop(const char *selfAddressStr, const char *peerAddressStr)
{
	// Create and configure the client socket
	UDPSocketPtr clientSocket = SocketUtil::CreateUDPSocket(INET);

	//clientSocket->SetNonBlockingMode(true);

	auto res = clientSocket->SetReuseAddress(true);
	if (res != NO_ERROR) { return; }

	SocketAddressPtr selfAddress = SocketAddressFactory::CreateIPv4FromString(selfAddressStr);
	res = clientSocket->Bind(*selfAddress);
	if (res != NO_ERROR) { return; }

	// Create the peer address
	SocketAddress fromAddress;
	SocketAddressPtr peerAddress = SocketAddressFactory::CreateIPv4FromString(peerAddressStr);

	char inBuffer[MTU];
	char outBuffer[MTU];
	char optionBuffer[8];

	while (1)
	{


		std::cout << std::endl;
		std::cout << "Menu" << std::endl;
		std::cout << "1) Send a message" << std::endl;
		std::cout << "2) Receive messages" << std::endl;
		std::cout << "Choose an option: " << std::flush;

		std::cin.getline(optionBuffer,sizeof(optionBuffer));

		if (strcmp(optionBuffer, "1") == 0)
		{
			// Send information
			std::cout << "Type the message: " << std::flush;
			std::cin.getline(outBuffer,sizeof(outBuffer));
			auto outLen = strlen(outBuffer);
			if (outLen > 0) {
				auto sentBytes = clientSocket->SendTo(outBuffer, outLen+1, *peerAddress);
			}
		}
		else if (strcmp(optionBuffer, "2") == 0)
		{
			// Receive information
			auto recvBytes = clientSocket->ReceiveFrom(inBuffer, MTU, fromAddress);
			if (recvBytes > 0) {
				std::cout << inBuffer <<std::endl;
			}
		}
		else
		{
			std::cout << "Unknown option" << std::endl;
		}

	}
}

#if _WIN32
int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	if (argc != 2) {
		StringUtils::Log("Usage: %s <peer_ip_address>:<port>", __argv[0]);
		return -1;
	}

	mainLoop(__argv[1]);

	return 0;
}
#else
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;

	if (argc != 3) {
		StringUtils::Log("Usage: <self_if_address>:<port> <peer_ip_address>:<port>", __argv[0]);
		return -1;
	}

	mainLoop(__argv[1], __argv[2]);

	return 0;
}
#endif

