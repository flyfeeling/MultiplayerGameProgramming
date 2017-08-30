#ifndef SOCKET_ADDRESS_FACTORY_H
#define SOCKET_ADDRESS_FACTORY_H

class SocketAddressFactory
{
public:

	static SocketAddressPtr CreateIPv4FromString(const std::string &inString);
};

#endif // SOCKET_ADDRESS_FACTORY_H
