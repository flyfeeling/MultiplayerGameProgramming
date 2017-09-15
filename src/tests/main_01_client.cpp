#include "Multiplayer.h"

const int MTU = 1300;

void client(const char *clientName)
{
	// Client string
	const std::string CLIENT_STRING = StringUtils::Sprintf("Hello from client: %s", clientName);

	// Client socket
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(INET);

//	SocketAddressPtr bindAddress;
//	auto res = clientSocket->Bind(*bindAddress);
//	if (res != NO_ERROR) { return; }

	// Connect to server
	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	auto res = clientSocket->Connect(*serverAddress);
	if (res != NO_ERROR) { return; }

	std::vector<TCPSocketPtr> sockets;
	std::vector<TCPSocketPtr> inSocketsWrite;
	std::vector<TCPSocketPtr> outSocketsRead;
	std::vector<TCPSocketPtr> outSocketsWrite;
	std::vector<TCPSocketPtr> outSocketsExcept;
	std::vector<TCPSocketPtr> towrite;
	std::set<TCPSocketPtr> written;
	std::set<TCPSocketPtr> disconnected;
	std::vector<TCPSocketPtr> connected;

	sockets.push_back(clientSocket);
	inSocketsWrite.push_back(clientSocket);

	char inBuffer[MTU];

	while (1)
	{
		StringUtils::Log("---");

		int res = SocketUtil::Select(&sockets, &outSocketsRead, &inSocketsWrite, &outSocketsWrite, &sockets, &outSocketsExcept);

		if (res < 0) {
			SocketUtil::ReportError("SocketUtil::Select");
		} else if (res == 0) {
			StringUtils::Log("There are no sockets selected", 0);
		} else {
			StringUtils::Log("There are %d sockets selected: read (%d) -  writing (%d) -  except (%d)",
					res,
					outSocketsRead.size(),
					outSocketsWrite.size(),
					outSocketsExcept.size());

			// Handle sockets to read
			for (auto it = outSocketsRead.begin(); it != outSocketsRead.end(); ++it)
			{
				TCPSocketPtr socket = *it;
				auto bytesRead = socket->Receive(inBuffer, MTU);
				if (bytesRead == 0) {
					// Handle disconnection
					StringUtils::Log("Socket disconnected");
					disconnected.insert(socket);
				} else {
					// Handle read data
					StringUtils::Log("Message received from server socket: %s", inBuffer);
					inSocketsWrite.push_back(socket); // respond to this socket
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

			// Handle sockets to write to
			for (auto it = outSocketsWrite.begin(); it != outSocketsWrite.end(); ++it)
			{
				TCPSocketPtr socket = *it;
				socket->Send(CLIENT_STRING.c_str(), CLIENT_STRING.size()+1);
				written.insert(socket);
			}

			// Handle sockets with exceptions
			for (auto it = outSocketsExcept.begin(); it != outSocketsExcept.end(); ++it)
			{
				TCPSocketPtr socket = *it;
				// TODO: handle exception
				StringUtils::Log("Exception on socket", 0);
			}

			// Save non-written sockets
			for (auto it = inSocketsWrite.begin(); it != inSocketsWrite.end(); ++it)
			{
				TCPSocketPtr socket = *it;
				if (written.find(socket) == written.end()) {
					towrite.push_back(socket);
				}
			}
			inSocketsWrite.swap(towrite);
			towrite.clear();
			written.clear();
		}

		// Wait one second
		const int millis = 500;
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

