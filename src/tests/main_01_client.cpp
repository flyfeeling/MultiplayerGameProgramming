#include "Multiplayer.h"

void client()
{
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(INET);

	SocketAddressPtr bindAddress;
	clientSocket->Bind(*bindAddress);

	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	clientSocket->Connect(*serverAddress);

	std::vector<TCPSocketPtr> sockets;
	sockets.push_back(clientSocket);

	std::vector<TCPSocketPtr> socketsOutRead;
	std::vector<TCPSocketPtr> socketsOutWrite;
	std::vector<TCPSocketPtr> socketsOutExcept;

	char inBuffer[4092];

	while (1)
	{
		int res = SocketUtil::Select(&sockets, &socketsOutRead, &sockets, &socketsOutWrite, &sockets, &socketsOutExcept);

		if (res < 0) {
			SocketUtil::ReportError("SocketUtil::Select");
		} else if (res == 0) {
			StringUtils::Log("There are no sockets selected", 0);
		} else {
			StringUtils::Log("There are %d sockets selected: sockets for read (%d) - sockets for writing (%d) - sockets with except (%d)", res, socketsOutRead.size(), socketsOutWrite.size(), socketsOutExcept.size());
		}

		for (auto it = socketsOutRead.begin(); it != socketsOutRead.end(); ++it)
		{
			TCPSocketPtr socket = *it;
			socket->Receive(inBuffer, 4092);
		}

		for (auto it = socketsOutWrite.begin(); it != socketsOutWrite.end(); ++it)
		{
			TCPSocketPtr socket = *it;
			socket->Send("caca", 4);
		}

		for (auto it = socketsOutExcept.begin(); it != socketsOutExcept.end(); ++it)
		{
			TCPSocketPtr socket = *it;
			// TODO: handle exception
		}

		socketsOutRead.clear();

		const int millis = 1000;
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

	client();
}
#else
const char** __argv;
int __argc;
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;

	client();
}
#endif

