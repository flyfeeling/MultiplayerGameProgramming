#include "Multiplayer.h"

void server()
{
	TCPSocketPtr serverSocket = SocketUtil::CreateTCPSocket(INET);

	SocketAddressPtr bindAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	serverSocket->Bind(*bindAddress);

	serverSocket->Listen();

	std::vector<TCPSocketPtr> sockets;
	sockets.push_back(serverSocket);

	std::vector<TCPSocketPtr> socketsOutRead;

	while (1)
	{
		int res = SocketUtil::Select(&sockets, &socketsOutRead, nullptr, nullptr, nullptr, nullptr);

		if (res < 0) {
			SocketUtil::ReportError("SocketUtil::Select");
		} else if (res == 0) {
			StringUtils::Log("There are no sockets selected", 0);
		} else {
			StringUtils::Log("There are %d sockets selected: sockets for read (%d)", res, socketsOutRead.size());
		}

		for (auto it = socketsOutRead.begin(); it != socketsOutRead.end(); ++it)
		{
			TCPSocketPtr socket = *it;
			if (socket == serverSocket)
			{
				StringUtils::Log("Accept connection", 0);
				SocketAddress fromAddress;
				TCPSocketPtr clientSocket = socket->Accept(fromAddress);
				sockets.push_back(clientSocket);
			}
		}

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
}
#else
const char** __argv;
int __argc;
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;

	server();
}
#endif

