#include "Multiplayer.h"

const int MTU = 1300;

void client(const char *clientName)
{
	// Client string
	const std::string CLIENT_STRING = StringUtils::Sprintf("Hello from client: %s", clientName);

	// Client socket
	UDPSocketPtr clientSocket = SocketUtil::CreateUDPSocket(INET);

	clientSocket->SetNonBlockingMode(true);

	auto res = clientSocket->SetReuseAddress(true);
	if (res != NO_ERROR) {
		SocketUtil::ReportError("Could not set socket option SO_REUSEADDR");
	}

	SocketAddressPtr bindAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8081");
	res = clientSocket->Bind(*bindAddress);
	if (res != NO_ERROR) { return; }

	SocketAddress fromAddress;

	char inBuffer[MTU];

	// We first say hello to the server
	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	auto sentBytes = clientSocket->SendTo(CLIENT_STRING.c_str(), CLIENT_STRING.size()+1, *serverAddress);

	while (1)
	{
		StringUtils::Log("---");

		// Try read
		auto recvBytes = clientSocket->ReceiveFrom(inBuffer, MTU, fromAddress);
		if (recvBytes > 0)
		{
			StringUtils::Log("Received from client: %s", inBuffer);

			auto sentBytes = clientSocket->SendTo(CLIENT_STRING.c_str(), CLIENT_STRING.size()+1, fromAddress);
		}


		// Wait one second
		const int millis = 2000;
#ifdef _WIN32
		Sleep(millis);
#else
		usleep(1000 * millis);
#endif
	}
}

#if _WIN32
int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	if (argc != 2) {
		StringUtils::Log("Usage: %s <client_name>", __argv[0]);
		return -1;
	}

	client(__argv[1]);

	return 0;
}
#else
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;

	if (argc != 2) {
		StringUtils::Log("Usage: %s <client_name>", __argv[0]);
		return -1;
	}

	client(__argv[1]);

	return 0;
}
#endif

