#include "Multiplayer.h"

SocketAddress::SocketAddress(const std::string &inString)
{
	// Parse inString
	const auto pos = inString.find_last_of(':');
	std::string host, service;
	if (pos != std::string::npos)
	{
		host = inString.substr(0, pos);
		service = inString.substr(pos + 1);
	}
	else
	{
		host = inString;
		service = "0"; // default port...
	}

	// Hint to specify we want connections via IPv4
	addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET; // IPv4

	// Get address information into result
	addrinfo *result;
	const int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
	addrinfo *initResult = result;

	if (error == 0)
	{
		// Search the first valid addrinfo
		while (!result->ai_addr && result->ai_next)
		{
			result = result->ai_next;
		}

		// And copy its value
		if (result && result->ai_addr) {
			const sockaddr &addr(*result->ai_addr);
			memcpy(&mSockAddr, &addr, sizeof(addr));
		}
	}

	// Release getaddrinfo results
	freeaddrinfo(initResult);
}

