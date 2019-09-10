#ifndef _cBitmapLoader_HG_
#define _cBitmapLoader_HG_

#include <fstream>
#include <vector>


class cBitmapLoader
{
public:
	cBitmapLoader();
	~cBitmapLoader();

	struct sBMPFileHeader 
	{
		sBMPFileHeader();

		unsigned short headerField;
		unsigned long fileSize;
		unsigned short reserved1;
		unsigned short reserved2;
		unsigned long dataOffset;
	};

	struct sBMPInfoHeader
	{
		sBMPInfoHeader();

		unsigned long sizeOfHeader;
		long imageWidth;
		long imageHeight;
		unsigned short numColorPlanes;
		unsigned short bitsPerPixel;
		unsigned short compressionMethod;
		unsigned long imageSize;
		unsigned long horizontalResolution;
		unsigned long verticalResolution;
		unsigned long numColorsInPalette;
		unsigned long numImportantColors;

		char* bitmapArray;
		std::vector<int> vecBitmap;
	};

	bool OpenBinaryFile(std::string filename);
	void ReadData(unsigned int count, bool reverseColour = true);

	std::ifstream stream;
	sBMPFileHeader* bmpHeader;
	sBMPInfoHeader* bmpInfo;

private:
	std::string _basePath;

};


#endif // !_cBitmapLoader_HG_


// Usage Example:
//BMPFileHeader fileHeader;
//BMPInfoHeader info;
//OpenBinaryFile("example.bmp");
//ReadData(&fileHeader, sizeof(fileHeader);
//ReadData(&info, sizeof(BMPInfoHeader );
//unsigned char* imageData = new char[info.imageSize]
//ReadData(&imageData, info.imageSize);
//
//imageData[0] = R
//imageData[1] = G
//imageData[2] = B

