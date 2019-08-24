#ifndef _cDIYMap_HG_
#define _cDIYMap_HG_

#include "iPersonMotron.h"


struct sPerson;
class cDIYMap : public iPersonMotron
{
public:
	cDIYMap();
	~cDIYMap();

	// Adds an element to the end of the vector
	void push_back(const std::string& index, void* data);

	// Returns element at the index
	void* at(const std::string& index, void* outData);

	// Returns current vector size
	unsigned long size();

	// Loads data into the "main" container.
	// Whenever this is called, any "old" data is destroyed.
	virtual bool LoadDataFilesIntoContainer(std::string firstNameFemaleFileName,
		std::string firstNameMaleFileName,
		std::string lastNameFileName);

	// Returns only people with this name 
	// If the name (first or last) is blank (""), then ignore this parameter.
	// For example: 
	// - if last name is "", search only by first name
	// - if first name is "", search only by last name
	// - if both names are blank, return everyone
	virtual bool FindPeopleByName(std::vector<sPerson> &vecPeople, sPerson personToMatch, int maxNumberOfPeople = INT_MAX);
	virtual bool FindPeopleByName(std::vector<sPerson> &vecPeople, std::vector<sPerson> &vecPeopleToMatch, int maxNumberOfPeople = INT_MAX);

	// Returns false if not found.
	virtual bool FindPersonByID(sPerson &person, unsigned long long uniqueID);

	// Finds the closest people (could be zero), from a point and radius. 
	// Assume that location is "less than or equal" to radius
	virtual bool FindPeople(std::vector<sPerson> &vecPeople, sPoint location, float radius,
		int maxPeopleToReturn = INT_MAX);
	// Finds people with a cetain health range (inclusive of the min and max values)
	virtual bool FindPeople(std::vector<sPerson> &vecPeople, float minHealth, float maxHealth,
		int maxPeopleToReturn = INT_MAX);
	// Combination of the two functions above
	virtual bool FindPeople(std::vector<sPerson> &vecPeople,
		sPoint location, float radius, float minHealth, float maxHealth,
		int maxPeopleToReturn = INT_MAX);

	// Sorts the people, then returns the container of sorted people
	// - this is from the data loaded by LoadDataFilesIntoContainer()
	virtual bool SortPeople(std::vector<sPerson> &vecPeople, eSortType sortType);

	// Can be called after every function. 
	// Calls from any of the above functions overwrites this data. 
	// Measuring starts from when call is made to just before it returns.
	virtual bool GetPerformanceFromLastCall(sPerfData &callStats);

	// Returns the enum from the cPerson.h file
	virtual eContainerType getContainerType(void);

private:
	void _InitArray();
	unsigned int _GenerateHash(const std::string& index);

	void** _pMap;
	sPerson* _pCurData;

	// Vector params
	static unsigned int INIT_CAPACITY; // How much can the vector hold when created
	unsigned int _curCapacity; // How much can the vector hold currently
	unsigned long _curSize;
	unsigned int _nextIndex; // Next viable index of the vector

};


#endif // !_cDIYMap_HG_

