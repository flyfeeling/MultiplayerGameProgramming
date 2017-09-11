#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

class TCPSocket;

typedef std::shared_ptr<TCPSocket> TCPSocketPtr;

class TCPSocket
{
public:

	~TCPSocket();
	
	int Bind(const SocketAddress &inToAddress);
	int Listen(int inBackLog = 32);
	TCPSocketPtr Accept(SocketAddress &inFromAddress);
	int Connect(const SocketAddress &inAddress);
	int Send(const void *inData, int inLen);
	int Receive(void *inBuffer, int inLen);

	int SetNonBlockingMode(bool inShouldBeNonBlocking);
	int SetReuseAddress(bool inShouldReuseAddress);

private:

	friend class SocketUtil;
	TCPSocket(SOCKET inSocket) : mSocket(inSocket) { }
	SOCKET mSocket;
};

#endif // TCP_SOCKET_H
