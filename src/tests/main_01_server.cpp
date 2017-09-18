#include "Multiplayer.h"

const int MTU = 1300;

static const char *SERVER_STRING = "Hello from server";

void server()
{
	TCPSocketPtr serverSocket = SocketUtil::CreateTCPSocket(INET);

	auto res = serverSocket->SetReuseAddress(true);
	if (res != NO_ERROR) {
		SocketUtil::ReportError("Could not set socket option SO_REUSEADDR");
	}

	SocketAddress bindAddress("127.0.0.1:8080");
	res = serverSocket->Bind(bindAddress);
	if (res != NO_ERROR) { return; }

	res = serverSocket->Listen();
	if (res != NO_ERROR) { return; }

	std::vector<TCPSocketPtr> sockets;
	sockets.push_back(serverSocket);

	std::vector<TCPSocketPtr> outSocketsRead;
	std::vector<TCPSocketPtr> outSocketsWrite;
	std::vector<TCPSocketPtr> outSocketsExcept;
	std::vector<TCPSocketPtr> inSocketsWrite;
	std::vector<TCPSocketPtr> towrite;
	std::vector<TCPSocketPtr> connected;
	std::set<TCPSocketPtr> written;
	std::set<TCPSocketPtr> disconnected;

	char inBuffer[MTU];

	while (1)
	{
		StringUtils::Log("---");
		int res = SocketUtil::Select(&sockets, &outSocketsRead, &inSocketsWrite, &outSocketsWrite, &sockets, &outSocketsExcept);

		if (res < 0) {
			SocketUtil::ReportError("SocketUtil::Select");
		} else if (res == 0) {
			StringUtils::Log("There are no sockets selected");
		} else {
			StringUtils::Log(
					"There are %d sockets selected: sockets for read (%d) - write (%d)",
					res,
					outSocketsRead.size(),
					outSocketsWrite.size(),
					outSocketsExcept.size());

			// Handle sockets ready to be read
			for (auto it = outSocketsRead.begin(); it != outSocketsRead.end(); ++it)
			{
				TCPSocketPtr socket = *it;
				if (socket == serverSocket)
				{
					StringUtils::Log("Accept connection");
					SocketAddress fromAddress;
					TCPSocketPtr clientSocket = socket->Accept(fromAddress);
					if (clientSocket == nullptr) {
						StringUtils::Log("There was an error accepting the client socket");
					} else {
						sockets.push_back(clientSocket);
					}
				}
				else
				{
					auto recvBytes = socket->Receive(inBuffer, MTU);
					if (recvBytes == 0)
					{
						// Handle disconnection
						StringUtils::Log("Socket disconnected");
						disconnected.insert(socket);
					}
					else
					{
						// Handle read data	
						StringUtils::Log("Socket received: %s", inBuffer);
						inSocketsWrite.push_back(socket);
					}
				}
			}

			// Remove disconnected sockets
			for (auto it = sockets.begin(); it != sockets.end(); ++it)
			{
				TCPSocketPtr socket = *it;
				if (disconnected.find(socket) == disconnected.end()) {
					connected.push_back(socket);
				}
			}
			sockets.swap(connected);
			connected.clear();
			disconnected.clear();

			outSocketsRead.clear();

			// Handle sockets ready to be written to
			for (auto it = outSocketsWrite.begin(); it != outSocketsWrite.end(); ++it)
			{
				TCPSocketPtr socket = *it;
				socket->Send(SERVER_STRING, strlen(SERVER_STRING)+1);
				written.insert(socket);
			}
			outSocketsWrite.clear();

			// Save non-written sockets
			for (auto it = inSocketsWrite.begin(); it != inSocketsWrite.end(); ++it)
			{
				TCPSocketPtr socket = *it;
				if (written.find(socket) == written.end()) {
					towrite.push_back(socket);
				}
			}
			inSocketsWrite.swap(towrite);
			written.clear();
			towrite.clear();
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

