#include "utility.h"
using namespace std;

void getDirectoryFromFile(string& filePath)
{
	const size_t last_slash_index = filePath.rfind('/');
	if (string::npos != last_slash_index)
	{
		filePath = filePath.substr(0, last_slash_index);
	}
}

std::string unix_time_to_date(const time_t& unix_time)
{ 
    return ctime(&unix_time);
}


