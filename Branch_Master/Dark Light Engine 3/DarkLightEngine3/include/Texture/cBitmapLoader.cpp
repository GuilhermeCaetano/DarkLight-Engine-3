// cBitmapLoader.cpp

#include <Texture\cBitmapLoader.h>


cBitmapLoader::cBitmapLoader()
{
	this->_basePath = "assets/textures/";
	this->bmpHeader = nullptr;
	this->bmpInfo = nullptr;

	return;
}

cBitmapLoader::~cBitmapLoader()
{
	delete this->bmpHeader;
	delete[] this->bmpInfo->bitmapArray;
	delete this->bmpInfo;

	return;
}

cBitmapLoader::sBMPFileHeader::sBMPFileHeader()
{
	this->headerField = 0;
	this->fileSize = 0;
	this->reserved1 = 0;
	this->reserved2 = 0;
	this->dataOffset = 0;
}

cBitmapLoader::sBMPInfoHeader::sBMPInfoHeader()
{
	this->sizeOfHeader = 0;
	this->imageWidth = 0;
	this->imageHeight = 0;
	this->numColorPlanes = 0;
	this->bitsPerPixel = 0;
	this->compressionMethod = 0;
	this->imageSize = 0;
	this->horizontalResolution = 0;
	this->verticalResolution = 0;
	this->numColorsInPalette = 0;
	this->numImportantColors = 0;
}

bool cBitmapLoader::OpenBinaryFile(std::string filename)
{
	// Open file as binary is easier to read chunks of data:
	std::string fullPath = this->_basePath;
	fullPath.append(filename);
	stream.open(fullPath, std::ios_base::binary);
	if (!stream.is_open())
	{
		return false;
	}

	return true;
}

void cBitmapLoader::ReadData(unsigned int count, bool reverseColour)
{
	// Open file as binary is easier to read chunks of data:
	char* initialRead = new char[count];
	stream.read(initialRead, count);

	if (this->bmpHeader != nullptr)
	{
		delete this->bmpHeader;
		this->bmpHeader = nullptr;
	}
	if (this->bmpInfo != nullptr)
	{
		delete this->bmpInfo;
		this->bmpInfo = nullptr;
	}

	// Read the bitmap header, this is the first 14 bytes
	this->bmpHeader = new sBMPFileHeader();
	this->bmpHeader->headerField = initialRead[0]; // header field (0-2) bytes
	this->bmpHeader->fileSize = initialRead[2]; // size (2-6) bytes
	this->bmpHeader->reserved1 = initialRead[6]; // reserved 1 (6-8) bytes
	this->bmpHeader->reserved2 = initialRead[8]; // reserved 2 (8-10) bytes
	this->bmpHeader->dataOffset = initialRead[10]; // offset (10-14) bytes

	// Read the info header, this is the rest of the 54 header bytes
	this->bmpInfo = new sBMPInfoHeader();
	this->bmpInfo->sizeOfHeader = initialRead[14]; // (14-18) bytes
	this->bmpInfo->imageWidth = initialRead[18]; // (18-22) bytes
	this->bmpInfo->imageHeight = initialRead[22]; // (22-26) bytes
	this->bmpInfo->numColorPlanes = initialRead[26]; // (26-28) bytes
	this->bmpInfo->bitsPerPixel = initialRead[28]; // (28-30) bytes
	this->bmpInfo->compressionMethod = initialRead[30]; // (30-34) bytes
	this->bmpInfo->imageSize = initialRead[34]; // (34-38) bytes
	this->bmpInfo->horizontalResolution = initialRead[38]; // (38-42) bytes
	this->bmpInfo->verticalResolution = initialRead[42]; // (42-46) bytes
	this->bmpInfo->numColorsInPalette = initialRead[46]; // (46-50) bytes
	this->bmpInfo->numImportantColors = initialRead[50]; // (50-54) bytes

	delete initialRead;

	// Must be careful with padding
	unsigned long paddedSize = ((this->bmpInfo->imageWidth * 3 + 3) & (~3)) * this->bmpInfo->imageHeight;
	this->bmpInfo->bitmapArray = new char[paddedSize];
	//this->bmpInfo->bitmapIntArray = new int[paddedSize];

	// Read the rest of the bitmap
	stream.read(this->bmpInfo->bitmapArray, paddedSize);

	// Reverse from BGR to RGB if wanted
	if (reverseColour)
	{
		// We need to reverse from BGR (windows format) to RGB (more default format).
		// Since G will always be the same, we swap only B and R
		for (long i = paddedSize - 3; i >= 0; i -= 3)
		{
			char temp = this->bmpInfo->bitmapArray[i];
			this->bmpInfo->bitmapArray[i] = this->bmpInfo->bitmapArray[i + 2];
			this->bmpInfo->bitmapArray[i + 2] = temp;
		}

		// Flip Image
		for (long i = paddedSize - 3; i >= 0; i -= 3)
		{
			this->bmpInfo->vecBitmap.push_back(int(this->bmpInfo->bitmapArray[i] & 0xff));
			this->bmpInfo->vecBitmap.push_back(int(this->bmpInfo->bitmapArray[i + 1] & 0xff));
			this->bmpInfo->vecBitmap.push_back(int(this->bmpInfo->bitmapArray[i + 2] & 0xff));
		}
	}
}
