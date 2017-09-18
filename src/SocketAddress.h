#ifndef SOCKET_ADDRESS_H
#define SOCKET_ADDRESS_H

class SocketAddress
{
public:

	SocketAddress(uint16_t inPort)
	{
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetAsSockAddrIn()->sin_addr.s_addr = INADDR_ANY;
		GetAsSockAddrIn()->sin_port = inPort;
	}

	SocketAddress(uint32_t inAddress, uint16_t inPort)
	{
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetAsSockAddrIn()->sin_addr.s_addr = htonl(inAddress);
		GetAsSockAddrIn()->sin_port = htons(inPort);
	}
	
	SocketAddress(const sockaddr &inSockAddr)
	{
		memcpy(&mSockAddr, &inSockAddr, sizeof(sockaddr));
	}

	SocketAddress()
	{
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetAsSockAddrIn()->sin_addr.s_addr = INADDR_ANY;
		GetAsSockAddrIn()->sin_port = 0;
	}

	size_t GetSize() const { return sizeof(sockaddr); }

	bool operator==(const SocketAddress &s) const
	{
		const auto *sa1 = GetAsSockAddrIn();
		const auto *sa2 = s.GetAsSockAddrIn();
		return
			sa1->sin_family == sa2->sin_family &&
			sa1->sin_addr.s_addr == sa2->sin_addr.s_addr &&
			sa1->sin_port == sa2->sin_port;
	}

	bool operator<(const SocketAddress &s) const
	{
		const auto *sa1 = GetAsSockAddrIn();
		const auto *sa2 = s.GetAsSockAddrIn();
		return (sa1->sin_family < sa2->sin_family) ||
			(sa1->sin_family == sa2->sin_family && sa1->sin_addr.s_addr < sa2->sin_addr.s_addr) ||
			(sa1->sin_family == sa2->sin_family && sa1->sin_addr.s_addr == sa2->sin_addr.s_addr && sa1->sin_port < sa2->sin_port);
	}

private:

	friend class TCPSocket;
	friend class UDPSocket;

	sockaddr mSockAddr;

	sockaddr_in* GetAsSockAddrIn()
	{
		return reinterpret_cast<sockaddr_in*>(&mSockAddr);
	}

	const sockaddr_in* GetAsSockAddrIn() const
	{
		return reinterpret_cast<const sockaddr_in*>(&mSockAddr);
	}
};

typedef std::shared_ptr<SocketAddress> SocketAddressPtr;

#endif // SOCKET_ADDRESS_H
