#ifndef _cPersonMotron_HG_
#define _cPersonMotron_HG_

#include "cPerson.h"
#include <string>
#include <vector>

// *************************************************************
//	   _          __              _   _             _ 
//	  /_\   ___  / _|_  _ _ _  __| |_(_)___ _ _  __(_)
//	 / _ \ (_-< |  _| || | ' \/ _|  _| / _ \ ' \(_-<_ 
//	/_/ \_\/__/ |_|  \_,_|_||_\__|\__|_\___/_||_/__(_)
//	                                                  
// *************************************************************

// Loads data into the "main" container.
// Whenever this is called, any "old" data is destroyed.
bool LoadDataFilesIntoContainer( std::string firstNameFemaleFileName, 
								 std::string firstNameMaleFileName, 
								 std::string lastNameFileName );

// Returns only people with this name 
// If the name (first or last) is blank (""), then ignore this parameter.
// For example: 
// - if last name is "", search only by first name
// - if first name is "", search only by last name
// - if both names are blank, return everyone
bool FindPeopleByName( std::vector<sPerson> &vecPeople, sPerson personToMatch, int maxNumberOfPeople = INT_MAX );
bool FindPeopleByName( std::vector<sPerson> &vecPeople, std::vector<sPerson> &vecPeopleToMatch, int maxNumberOfPeople = INT_MAX );

// Returns false if not found.
bool FindPersonByID( sPerson &person, unsigned long long uniqueID );

// Finds the closest people (could be zero), from a point and radius. 
// Assume that location is "less than or equal" to radius
bool FindPeople( std::vector<sPerson> &vecPeople, sPoint location, float radius, 
				 int maxPeopleToReturn = INT_MAX );
// Finds people with a cetain health range (inclusive of the min and max values)
bool FindPeople( std::vector<sPerson> &vecPeople, float minHealth, float maxHealth, 
				 int maxPeopleToReturn = INT_MAX );
// Combination of the two functions above
bool FindPeople( std::vector<sPerson> &vecPeople, 
				 sPoint location, float radius, float minHealth, float maxHealth, 
				 int maxPeopleToReturn = INT_MAX );

// Returns the enum from the cPerson.h file
eContainerType getContainerType(void);

enum eSortType
{
	ASC_FIRST_THEN_LAST,
	DESC_FIRST_THEN_LAST,
	ASC_LAST_THEN_FIRST,
	DESC_LAST_THEN_FIRST,
	ASC_BY_ID,
	DESC_BY_ID,
	ASC_BY_HEALTH,
	DESC_BY_HEALTH
};

// Sorts the people, then returns the container of sorted people
// - this is from the data loaded by LoadDataFilesIntoContainer()
bool SortPeople( std::vector<sPerson> &vecPeople, eSortType sortType );

// Can be called after every function. 
// Calls from any of the above functions overwrites this data. 
// Measuring starts from when call is made to just before it returns.
bool GetPerformanceFromLastCall( sPerfData &callStats );


// *************************************************************
//	   _                   _   _            _    _ 
//	  /_\   ___  _ __  ___| |_| |_  ___  __| |__(_)
//	 / _ \ (_-< | '  \/ -_)  _| ' \/ _ \/ _` (_-<_ 
//	/_/ \_\/__/ |_|_|_\___|\__|_||_\___/\__,_/__(_)
//	                                               
// *************************************************************

class iPersonMotron
{
public:
	// Loads data into the "main" container.
	// Whenever this is called, any "old" data is destroyed.
	virtual bool LoadDataFilesIntoContainer( std::string firstNameFemaleFileName, 
	                                         std::string firstNameMaleFileName, 
	                                         std::string lastNameFileName ) = 0;

	// Returns only people with this name 
	// If the name (first or last) is blank (""), then ignore this parameter.
	// For example: 
	// - if last name is "", search only by first name
	// - if first name is "", search only by last name
	// - if both names are blank, return everyone
	virtual bool FindPeopleByName( std::vector<sPerson> &vecPeople, sPerson personToMatch, int maxNumberOfPeople = INT_MAX ) = 0;
	virtual bool FindPeopleByName( std::vector<sPerson> &vecPeople, std::vector<sPerson> &vecPeopleToMatch, int maxNumberOfPeople = INT_MAX ) = 0;

	// Returns false if not found.
	virtual bool FindPersonByID( sPerson &person, unsigned long long uniqueID ) = 0;

	// Finds the closest people (could be zero), from a point and radius. 
	// Assume that location is "less than or equal" to radius
	virtual bool FindPeople( std::vector<sPerson> &vecPeople, sPoint location, float radius, 
	                         int maxPeopleToReturn = INT_MAX ) = 0;
	// Finds people with a cetain health range (inclusive of the min and max values)
	virtual bool FindPeople( std::vector<sPerson> &vecPeople, float minHealth, float maxHealth, 
	                         int maxPeopleToReturn = INT_MAX ) = 0;
	// Combination of the two functions above
	virtual bool FindPeople( std::vector<sPerson> &vecPeople, 
	                         sPoint location, float radius, float minHealth, float maxHealth, 
	                         int maxPeopleToReturn = INT_MAX ) = 0;

	enum eSortType
	{
		ASC_FIRST_THEN_LAST,
		DESC_FIRST_THEN_LAST,
		ASC_LAST_THEN_FIRST,
		DESC_LAST_THEN_FIRST,
		ASC_BY_ID,
		DESC_BY_ID,
		ASC_BY_HEALTH,
		DESC_BY_HEALTH
	};

	// Sorts the people, then returns the container of sorted people
	// - this is from the data loaded by LoadDataFilesIntoContainer()
	virtual bool SortPeople( std::vector<sPerson> &vecPeople, eSortType sortType ) = 0;

	// Can be called after every function. 
	// Calls from any of the above functions overwrites this data. 
	// Measuring starts from when call is made to just before it returns.
	virtual bool GetPerformanceFromLastCall( sPerfData &callStats ) = 0;

	// Returns the enum from the cPerson.h file
	virtual eContainerType getContainerType(void) = 0;

};

#endif // cPersonMotron
