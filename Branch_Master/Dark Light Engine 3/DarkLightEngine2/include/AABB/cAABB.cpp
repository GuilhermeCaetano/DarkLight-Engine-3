// cMyAABB.cpp

#include <AABB\cAABB.h>


cAABB::cAABB(glm::vec3 centerXYZ, float sideLength, std::string meshName)
{
	this->_centerXYZ = centerXYZ;
	this->_sideLength = sideLength;
	this->_meshName = meshName;
	this->bShouldDraw = false;

	return;
}

cAABB::~cAABB()
{
	return;
}

// Getters for the center, side length and ID
glm::vec3 cAABB::GetCenterXYZ()
{
	return this->_centerXYZ;
}

float cAABB::GetSideLength()
{
	return this->_sideLength;
}

std::string cAABB::GetMeshName()
{
	return this->_meshName;
}

unsigned long long cAABB::GetID()
{
	return this->GenerateID(this->_centerXYZ);
}

// Generate ID automatically from private variables
unsigned long long cAABB::GenerateID(glm::vec3 centerXYZ)
{
	// 16,000,000,100,000,100,000 
	//    +xx,xxx +yy,yyy +zz,zzz  
	// If the value is NEGATIVE, then we will set the 6th 
	// digit to 1. If +ve then, it's 0
	// for example, xyz of 20, 460, 1280 would give:
	// // 100020 000460 101280
	unsigned long long theABS_X = (unsigned long long(fabs(centerXYZ.x)));
	unsigned long long theABS_Y = (unsigned long long(fabs(centerXYZ.y)));
	unsigned long long theABS_Z = (unsigned long long(fabs(centerXYZ.z)));
	// Add the "sign" digit:
	// If +ve, then the sign is 0, eg: 193 would be:  000193   (000 193)
	// If -ve, then the sign is 1, eg: -193 would be: 100193   (100 193)
	if (centerXYZ.x < 0.0f) { theABS_X += 100000; } // Sets 6th digit to 1
	if (centerXYZ.y < 0.0f) { theABS_Y += 100000; } // Sets 6th digit to 1
	if (centerXYZ.z < 0.0f) { theABS_Z += 100000; } // Sets 6th digit to 1
	unsigned long long theID =
		theABS_Z
		+ (theABS_Y * 1000000) // Shift the y to the 7th digit
		+ (theABS_X * 1000000 * 1000000); // Shift the x to the 13th
	return theID;
}

// Generate ID checking the side length
unsigned long long cAABB::GenerateID(glm::vec3 centerXYZ, float sideLength)
{
	// By using the side length and floor, 
	centerXYZ.x = ((floor((centerXYZ.x - ((float)sideLength / 2.0f)) / sideLength)) * sideLength) + sideLength;
	centerXYZ.y = ((floor((centerXYZ.y - ((float)sideLength / 2.0f)) / sideLength)) * sideLength) + sideLength;
	centerXYZ.z = ((floor((centerXYZ.z - ((float)sideLength / 2.0f)) / sideLength)) * sideLength) + sideLength;

	return cAABB::GenerateID(centerXYZ);
}