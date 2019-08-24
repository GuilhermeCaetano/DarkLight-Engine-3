#ifndef _cSTLList_HG_
#define _cSTLList_HG_

#include "iPersonMotron.h"
#include <list>


struct sPerson;
class cSTLList : public iPersonMotron
{
public:
	cSTLList();
	~cSTLList();

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
	std::list<sPerson*> _listPeople;

};


#endif // !_cSTLList_HG_

