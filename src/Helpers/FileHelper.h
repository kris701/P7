#ifndef FileHelper_H
#define FileHelper_H

#include <filesystem>

using namespace std;

class FileHelper {
public:
	static bool DoesFileExist(filesystem::path fileName);
};

#endif