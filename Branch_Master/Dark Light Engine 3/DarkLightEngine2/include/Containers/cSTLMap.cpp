// cSTLMap.cpp

#include "cSTLMap.h"
#include <fstream>


cSTLMap::cSTLMap()
{
	return;
}

cSTLMap::~cSTLMap()
{
	return;
}

bool cSTLMap::LoadDataFilesIntoContainer(std::string firstNameFemaleFileName, std::string firstNameMaleFileName, std::string lastNameFileName)
{
	std::string path = "assets/txt/";
	std::string femaleName = path + firstNameFemaleFileName;
	std::string maleName = path + firstNameMaleFileName;
	std::string lastName = path + lastNameFileName;

	// They are, there's almost no use for something that can't be returned back.
	std::string arraysAreGarbage1[400];
	std::string arraysAreGarbage2[400];
	std::string arraysAreGarbage3[400];

	// Open file
	std::fstream fileF(femaleName);
	std::fstream fileM(maleName);
	std::fstream fileL(lastName);

	// Read 100 entries * 4 columns. No, I'm NOT reading 16000 entries.
	std::string word;
	unsigned int wordCount = 0;

	// Female
	if (!fileF.is_open())
	{
		printf("Error! File %s couldn't be opened!\n", femaleName);
	}
	while (fileF >> word)
	{
		arraysAreGarbage1[wordCount] = word;
		wordCount += 1;
		if (wordCount == 400)
		{
			wordCount = 0;
			break;
		}
	}

	// Male
	if (!fileM.is_open())
	{
		printf("Error! File %s couldn't be opened!\n", maleName);
	}
	while (fileM >> word)
	{
		arraysAreGarbage2[wordCount] = word;
		wordCount += 1;
		if (wordCount == 400)
		{
			wordCount = 0;
			break;
		}
	}

	// Last
	if (!fileL.is_open())
	{
		printf("Error! File %s couldn't be opened!\n", lastName);
	}
	while (fileL >> word)
	{
		arraysAreGarbage3[wordCount] = word;
		wordCount += 1;
		if (wordCount == 400)
		{
			wordCount = 0;
			break;
		}
	}

	// Female
	for (unsigned int i = 0; i < 400; i += 4)
	{
		sPerson* person = new sPerson();
		person->first = arraysAreGarbage1[i];
		person->last = arraysAreGarbage3[i];
		person->location.x = atof(arraysAreGarbage1[i + 1].c_str());
		person->health = atof(arraysAreGarbage1[i + 2].c_str());
		person->uniqueID = atoll(arraysAreGarbage1[i + 3].c_str());
		this->_mapPeopleByID[person->uniqueID] = person;
	}

	// Male
	for (unsigned int i = 0; i < 400; i += 4)
	{
		sPerson* person = new sPerson();
		person->first = arraysAreGarbage2[i];
		person->last = arraysAreGarbage3[i];
		person->location.x = atof(arraysAreGarbage2[i + 1].c_str());
		person->health = atof(arraysAreGarbage2[i + 2].c_str());
		person->uniqueID = atoll(arraysAreGarbage2[i + 3].c_str());
		this->_mapPeopleByID[person->uniqueID] = person;
	}

	return true;
}

bool cSTLMap::FindPeopleByName(std::vector<sPerson>& vecPeople, sPerson personToMatch, int maxNumberOfPeople)
{
	int numPeopleFound = 0;
	std::map<unsigned long long, sPerson*>::iterator mapIter = this->_mapPeopleByID.begin();

	for (mapIter; mapIter != this->_mapPeopleByID.end(); mapIter++)
	{
		sPerson* person = mapIter->second;

		if (person->first == personToMatch.first && person->last == personToMatch.last)
		{
			vecPeople.push_back(*person);
			numPeopleFound++;

			if (numPeopleFound >= maxNumberOfPeople)
			{
				return true;
			}
		}
	}

	return numPeopleFound > 0;
}

bool cSTLMap::FindPeopleByName(std::vector<sPerson>& vecPeople, std::vector<sPerson>& vecPeopleToMatch, int maxNumberOfPeople)
{
	int numPeopleFound = 0;
	std::map<unsigned long long, sPerson*>::iterator mapIter = this->_mapPeopleByID.begin();

	for (mapIter; mapIter != this->_mapPeopleByID.end(); mapIter++)
	{
		sPerson* pPerson = mapIter->second;
		for (unsigned int j = 0; j < vecPeopleToMatch.size(); j++)
		{
			sPerson person = vecPeopleToMatch[j];

			if (pPerson->first == person.first && pPerson->last == person.last)
			{
				numPeopleFound++;
				vecPeople.push_back(*pPerson);

				if (numPeopleFound >= maxNumberOfPeople)
				{
					return true;
				}
			}
		}
	}

	return numPeopleFound > 0;
}

bool cSTLMap::FindPersonByID(sPerson& person, unsigned long long uniqueID)
{
	std::map<unsigned long long, sPerson*>::iterator mapIter = this->_mapPeopleByID.begin();

	for (mapIter; mapIter != this->_mapPeopleByID.end(); mapIter++)
	{
		sPerson* pPerson = mapIter->second;

		if (pPerson->uniqueID == uniqueID)
		{
			person = *pPerson;
			return true;
		}
	}

	return false;
}

bool cSTLMap::FindPeople(std::vector<sPerson>& vecPeople, sPoint location, float radius, int maxPeopleToReturn)
{
	int numPeopleFound = 0;
	std::map<unsigned long long, sPerson*>::iterator mapIter = this->_mapPeopleByID.begin();

	for (mapIter; mapIter != this->_mapPeopleByID.end(); mapIter++)
	{
		sPerson* pPerson = mapIter->second;

		float distanceX = fabs(pPerson->location.x - location.x);
		float distanceY = fabs(pPerson->location.y - location.y);
		float distanceZ = fabs(pPerson->location.z - location.z);

		if (distanceX <= radius && distanceY <= radius && distanceZ <= radius)
		{
			vecPeople.push_back(*pPerson);
			numPeopleFound++;

			if (numPeopleFound >= maxPeopleToReturn)
			{
				return true;
			}
		}
	}

	return numPeopleFound > 0;
}

bool cSTLMap::FindPeople(std::vector<sPerson>& vecPeople, float minHealth, float maxHealth, int maxPeopleToReturn)
{
	int numPeopleFound = 0;
	std::map<unsigned long long, sPerson*>::iterator mapIter = this->_mapPeopleByID.begin();

	for (mapIter; mapIter != this->_mapPeopleByID.end(); mapIter++)
	{
		sPerson* pPerson = mapIter->second;

		if (pPerson->health >= minHealth && pPerson->health <= maxHealth)
		{
			vecPeople.push_back(*pPerson);
			numPeopleFound++;

			if (numPeopleFound >= maxPeopleToReturn)
			{
				return true;
			}
		}
	}

	return numPeopleFound > 0;
}

bool cSTLMap::FindPeople(std::vector<sPerson>& vecPeople, sPoint location, float radius, float minHealth, float maxHealth, int maxPeopleToReturn)
{
	int numPeopleFound = 0;
	std::map<unsigned long long, sPerson*>::iterator mapIter = this->_mapPeopleByID.begin();

	for (mapIter; mapIter != this->_mapPeopleByID.end(); mapIter++)
	{
		sPerson* pPerson = mapIter->second;

		float distanceX = fabs(pPerson->location.x - location.x);
		float distanceY = fabs(pPerson->location.y - location.y);
		float distanceZ = fabs(pPerson->location.z - location.z);

		bool isInRange = (distanceX <= radius && distanceY <= radius && distanceZ <= radius);
		bool healthRange = (pPerson->health >= minHealth && pPerson->health <= maxHealth);

		if (isInRange && healthRange)
		{
			vecPeople.push_back(*pPerson);
			numPeopleFound++;

			if (numPeopleFound >= maxPeopleToReturn)
			{
				return true;
			}
		}
	}

	return numPeopleFound > 0;
}

bool cSTLMap::SortPeople(std::vector<sPerson>& vecPeople, eSortType sortType)
{
	return false;
}

bool cSTLMap::GetPerformanceFromLastCall(sPerfData& callStats)
{
	return false;
}

eContainerType cSTLMap::getContainerType(void)
{
	return eContainerType::STD_MAP;
}
