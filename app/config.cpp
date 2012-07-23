// Configuration File Functions


#include "config.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

#define MAX_LINE_LENGTH 2048


Config::Config(const char *file)
{
	// Get Current Directory
	CHAR currentDir[MAX_PATH];
    GetModuleFileName(NULL, currentDir, MAX_PATH);					// JJL010424
    int len = strlen(currentDir);
    while (currentDir[len] != '\\')
        len--;
    currentDir[len+1] = 0;

	// Construct path\filename
	m_pFileName = new char[strlen(currentDir)+strlen(file)+1];
	strcpy(m_pFileName, currentDir);
	strcpy(&m_pFileName[len+1], file);

	m_Stream.open(m_pFileName, ios::in|ios::out);
}

Config::~Config()
{
	// Cleanup
	m_Stream.close();
	delete m_pFileName;
}


// Read a value from the INI file
bool Config::get(const char* section, const char* key,
				 char* value, const unsigned int maxlen)
{
	m_Stream.clear();
    m_Stream.seekg(0);	// Go to top


	char line[MAX_LINE_LENGTH], s1[256], s2[256], s3[1024];

	bool bInSection = false;
	while(!(m_Stream.eof() || m_Stream.fail()))		// Until the end of file
	{
		m_Stream.getline(line, 2048);

		int nParms = sscanf(line, "%s %s %[^\r\n]", s1, s2, s3);

		if(nParms > 0)	// Not a blank line
		{
			if(s1[0] == '[' && (s1[strlen(s1)-1] == ']') ) {	// if section header
				if(!strnicmp(&s1[1], section, strlen(s1)-2))
					bInSection = true;
				else
					bInSection = false;
			}
			else {
				// If this is the key we want
				if(bInSection && (nParms > 2) && !strcmp("=", s2) && !stricmp(s1, key))
				{
					unsigned int len = (unsigned int)strlen(s3);
					len = len < maxlen ? len : maxlen;
					strncpy(value, s3, len);
					value[len] = '\0';
					return true;
				}
			}
		}
	}
	return false;
}

// Add a value to the INI file
bool Config::put(const char* section, const char* key, const char* value)
{
	m_Stream.clear();
	m_Stream.seekg(0);	// Go to top

	char m_sTempFileName[MAX_PATH];
	sprintf(m_sTempFileName, "%s.tmp", m_pFileName);
	ofstream m_TempFile(m_sTempFileName, ios::out);

	char line[MAX_LINE_LENGTH], currentsection[256], s1[256], s2[256], s3[512];
	char newline[MAX_LINE_LENGTH];
	sprintf(newline, "%s = %s\0", key, value);

	bool bWritten = false;
	while(!(m_Stream.eof() || m_Stream.fail())) {
		m_Stream.getline(line, MAX_LINE_LENGTH);

		int nParms = sscanf(line, "%s %s %s", s1, s2, s3);

		if(nParms != -1) // > 0) would remove all blank lines, or lines with just spaces
		{
			if(s1[0] == '[' && (s1[strlen(s1)-1] == ']') ) {
                strncpy(currentsection, &s1[1], strlen(s1)-2);
                currentsection[strlen(s1)-2] = '\0';
				if(!stricmp(currentsection, section))
				{
					m_TempFile << line << '\n';
					m_TempFile << newline << '\n';
					bWritten = true;
				}
				else
					m_TempFile << line << '\n';
			}
			else {
				if(stricmp(s1, key) || stricmp(section, currentsection))
				{
//					if(strlen(line))
						m_TempFile << line << '\n';
				}
			}
		}
	}

	// If the appropriate section was not found, add it.
	if(!bWritten)
	{
		char sSection[MAX_LINE_LENGTH];
		sprintf(sSection, "[%s]\0", section);
		m_TempFile << '\n' << sSection << '\n';
		m_TempFile << newline << '\n';
	}

	// Delete our original file and replace it with the new version.
	m_TempFile.flush();	// Write buffer
	m_TempFile.close();
	m_Stream.close();

	DeleteFile(m_pFileName);
	MoveFile(m_sTempFileName, m_pFileName);

	m_Stream.open(m_pFileName, ios::in|ios::out);

	return true;
}
