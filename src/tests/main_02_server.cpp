#include "Multiplayer.h"

const int MTU = 1300;

static const char *SERVER_STRING = "Hello from server";

void server()
{
	UDPSocketPtr serverSocket = SocketUtil::CreateUDPSocket(INET);

	serverSocket->SetNonBlockingMode(true);

	auto res = serverSocket->SetReuseAddress(true);
	if (res != NO_ERROR) {
		SocketUtil::ReportError("Could not set socket option SO_REUSEADDR");
	}

	SocketAddressPtr bindAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	res = serverSocket->Bind(*bindAddress);
	if (res != NO_ERROR) { return; }

	SocketAddress fromAddress;

	char inBuffer[MTU];

	while (1)
	{
		StringUtils::Log("---");

		// Try read
		auto recvBytes = serverSocket->ReceiveFrom(inBuffer, MTU, fromAddress);
		if (recvBytes > 0)
		{
			StringUtils::Log("Received from client: %s", inBuffer);

			auto sentBytes = serverSocket->SendTo(SERVER_STRING, strlen(SERVER_STRING)+1, fromAddress);
		}

		// Wait half a second
		const int millis = 500;
#ifdef _WIN32
		Sleep(millis);
#else
		usleep(millis * 1000);
#endif
	}
}

#if _WIN32
int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	server();

	return 0;
}
#else
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;

	server();

	return 0;
}
#endif

