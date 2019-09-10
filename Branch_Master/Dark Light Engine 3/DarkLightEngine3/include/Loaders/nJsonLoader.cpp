// nJsonLoader.cpp

#pragma warning(disable:4996)

#include "nJsonLoader.h"

#include <fstream>
#include <rapidjson\filewritestream.h>
#include <rapidjson\filereadstream.h>


namespace nLoad
{
	inline bool FileExists(const char* fileName)
	{
		// We can check if the file exists by using the stat function and
		// checking the file status. Stat() seems to be the faster option.
		struct stat buffer;
		return (stat(fileName, &buffer) == 0);
	}

	// Create the first save file
	bool CreateNewFile(const std::string& filename)
	{
		// Otherwise, create one for us.
		std::ofstream newFile(filename);

		if (newFile.is_open())
		{
			printf("Created a new %s file.\n", filename.c_str());
			return true;
		}

		printf("Error, failed to create %s file!\n", filename.c_str());
		return false;
	}

	bool LoadJsonFile(const std::string& filename, rapidjson::Document& document, bool createIfInvalid)
	{
		// Check if the file exists, and if not,
		// create one if the user asked to.
		if (!FileExists(filename.c_str()) && createIfInvalid)
		{
			CreateNewFile(filename);
		}

		// Open file
		FILE* fp = fopen(filename.c_str(), "r");
		if (fp == nullptr)
		{
			printf("Failed to open %s json file.\n", filename.c_str());
			return false;
		}

		// Read and parse into a json DOM document
		char readBuffer[65536];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		document.ParseStream(is);

		if (!document.IsObject())
		{
			printf("Error, %s Json DOM is not an object!\n", filename.c_str());
			return false;
		}

		// Make sure the file is closed, it could cause problems.
		fclose(fp);
		return true;
	}

	//bool LoadTextFromFile(const char* filename)
	//{
	//	// Check file for validity
	//	if (!FileExists(filename))
	//	{
	//		// Error and bail out
	//		printf("Error! File %s couldn't be found!\n", filename);
	//	}

	//	// Open file
	//	std::fstream file(filename);

	//	// The file can only be opened if it exists
	//	if (!file.is_open())
	//	{
	//		printf("Error! File %s couldn't be opened!\n", filename);
	//	}

	//	std::string word = "new";
	//	unsigned int wordCount = 0;
	//	while (file >> word)
	//	{
	//		textLoad->arr[wordCount] = word;
	//		wordCount += 1;
	//		if (wordCount == 400)
	//		{
	//			break;
	//		}
	//	}

	//	return textLoad;
	//}
}

