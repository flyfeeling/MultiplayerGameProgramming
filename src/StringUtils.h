#ifndef STRING_UTILS_H
#define STRING_UTILS_H

namespace StringUtils
{
	std::string GetCommandLineArg(int inIndex);
	std::string Sprintf(const char* inFormat, ...);

	//void	Log(const char* inFormat);
	void	Log(const char* inFormat, ...);
}

#define LOG( ... ) StringUtils::Log( __VA_ARGS__ );

#endif // STRING_UTILS_H
