#ifndef _nJsonLoader_HG_
#define _nJsonLoader_HG_

#include <iostream>
#include <cstdio>
#include <rapidjson\document.h>


namespace nLoad
{
	struct sTextLoad
	{
		sTextLoad(unsigned int num)
		{
			arr[num];
		}

		std::string arr[];
	};

	// Inline can be used to increase performance on small functions
	// that would be called over and over again. However, it increases
	// file size and might lead to other problems. Shoudn't be used all the time.
	inline bool FileExists(const char* fileName);
	bool CreateNewFile(const std::string& filename);

	// Loads a .json file and returns the document by reference
	bool LoadJsonFile(const std::string& filename, rapidjson::Document& document, bool createIfInvalid = true);

	// Loads a .txt file and returns it by reference
	//bool LoadTextFromFile(const char* filename);

}


#endif // !_nJsonLoader_HG_

