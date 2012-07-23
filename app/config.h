// INI Configuration File Routines

#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>

using namespace std;

class Config
{
public:
	Config::Config(const char* file);
	Config::~Config();

	bool get(const char* section, const char* key,
				char* value, const unsigned int maxlen);
	bool put(const char* section, const char* key, const char* value);

private:
	char*	m_pFileName;
	fstream	m_Stream;
};

#endif