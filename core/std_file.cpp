// std_file:  Standard File Functions.
//////////////////////////////////////////////////////

#include "std_file.h"
#include <fstream>


// fileExists:  See if a file exists
//
bool fileExists(const char* sFile)
{
//	std::ifstream stream(sFile, std::ios::in|std::ios::nocreate);
////		filebuf::sh_read||filebuf::sh_write);
//	if(!stream.fail()) {							// File found
//		stream.close();
//		return true;
//	}
//
//	return false;

	FILE* inf = fopen(sFile,"rb");
	if(!inf) return false;
	fclose(inf);
	return true;
}
