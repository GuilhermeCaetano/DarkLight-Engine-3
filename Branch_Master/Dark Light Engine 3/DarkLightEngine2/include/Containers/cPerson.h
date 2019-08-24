#ifndef _cPerson_HG_
#define _cPerson_HG_

#include <string>
#include <vector>

enum eContainerType
{
	STD_VECTOR = 1, 
	STD_LIST,
	STD_MAP,
	CUSTOM_DIY_VECTOR,
	CUSTOM_DIY_LIST,
	CUSTOM_DIY_MAP,
	UNKNOWN
};

struct sPerfData
{
	sPerfData() : 
		elapsedCallTime_ms(0.0f), 
		memoryUsageBytes_min(0.0f), 
		memoryUsageBytes_max(0.0f),
		memoryUsageBytes_avg(0.0f) {}
	float elapsedCallTime_ms;
	float memoryUsageBytes_min;
	float memoryUsageBytes_max;
	float memoryUsageBytes_avg;
};

struct sPoint
{
	sPoint() : x(0.0f), y(0.0f), z(0.0f) {}
	float x, y, z;
};

struct sPerson
{
	sPerson() : uniqueID(0), age(0), health(0.0f) {}
	unsigned long long uniqueID;		// 64 bit unique identifier
	std::string first;
	std::string last;
	int age;
	float health;
	sPoint location;
};



#endif // _cPerson_HG_
