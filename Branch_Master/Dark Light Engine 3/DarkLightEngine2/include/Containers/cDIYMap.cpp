// cDIYMap.cpp

#include "cDIYMap.h"
#include <fstream>


unsigned int cDIYMap::INIT_CAPACITY = 100;

cDIYMap::cDIYMap()
{
	// Default size and Initial capacity
	this->_curCapacity = 100;
	this->_curSize = 0;
	this->_nextIndex = 0;

	this->_InitArray();

	return;
}

cDIYMap::~cDIYMap()
{
	return;
}

void cDIYMap::_InitArray()
{
	this->_pMap = new void*[this->_curCapacity];

	this->_pCurData = nullptr;

	// Innitialize whole vector to null
	for (int index = 0; index != this->_curCapacity; index++)
	{
		this->_pMap[index] = nullptr;
	}
}

void cDIYMap::push_back(const std::string& index, void* data)
{
	// Generate the hash for this object
	unsigned int indexHash = this->_GenerateHash(index);

	// Add to the map
	this->_pMap[indexHash] = data;
	this->_curSize++;

	if (this->_curSize > this->_curCapacity)
	{
		// Grow the array
		this->_curCapacity *= 2;

		// Make a new array 
		void** pNewArray = new void*[this->_curCapacity];

		// Copy the old data into the new one
		for (unsigned int index = 0; index < this->size(); index++)
		{
			pNewArray[index] = this->_pMap[index];
		}

		// Delete the old one
		delete[] this->_pMap;

		// Point the array to the new location
		this->_pMap = pNewArray;
	}
}

void* cDIYMap::at(const std::string& index, void* outData)
{
	// Generate the hash for this object
	unsigned int indexHash = this->_GenerateHash(index);

	if (this->_pMap == nullptr)
	{
		// map not initialized...
		return false;
	}

	outData = this->_pMap[indexHash];
}

unsigned long cDIYMap::size()
{
	return this->_curSize;
}

bool cDIYMap::LoadDataFilesIntoContainer(std::string firstNameFemaleFileName, std::string firstNameMaleFileName, std::string lastNameFileName)
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
		this->push_back(person->first, person);
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
		this->push_back(person->first, person);
	}

	return true;
}

bool cDIYMap::FindPeopleByName(std::vector<sPerson>& vecPeople, sPerson personToMatch, int maxNumberOfPeople)
{
	int numPeopleFound = 0;

	sPerson* person = nullptr;
	this->at(personToMatch.first, person);

	return person != nullptr;
}

bool cDIYMap::FindPeopleByName(std::vector<sPerson>& vecPeople, std::vector<sPerson>& vecPeopleToMatch, int maxNumberOfPeople)
{
	int numPeopleFound = 0;

	for (unsigned int i = 0; i < vecPeopleToMatch.size(); i++)
	{
		sPerson* pPerson = nullptr;
		this->at(vecPeopleToMatch[i].first, pPerson);
		
		if (pPerson != nullptr)
		{
			vecPeople.push_back(*pPerson);
			numPeopleFound++;
		}
	}

	return numPeopleFound > 0;
}

bool cDIYMap::FindPersonByID(sPerson& person, unsigned long long uniqueID)
{
	for (unsigned int i = 0; i < this->size(); i++)
	{
		sPerson* pPerson = static_cast<sPerson*>(this->_pMap[i]);

		if (pPerson->uniqueID == uniqueID)
		{
			person = *pPerson;
			return true;
		}
	}

	return false;
}

bool cDIYMap::FindPeople(std::vector<sPerson>& vecPeople, sPoint location, float radius, int maxPeopleToReturn)
{
	int numPeopleFound = 0;

	for (unsigned int i = 0; i < this->size(); i++)
	{
		sPerson* pPerson = static_cast<sPerson*>(this->_pMap[i]);

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

bool cDIYMap::FindPeople(std::vector<sPerson>& vecPeople, float minHealth, float maxHealth, int maxPeopleToReturn)
{
	int numPeopleFound = 0;

	for (unsigned int i = 0; i < this->size(); i++)
	{
		sPerson* pPerson = static_cast<sPerson*>(this->_pMap[i]);

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

bool cDIYMap::FindPeople(std::vector<sPerson>& vecPeople, sPoint location, float radius, float minHealth, float maxHealth, int maxPeopleToReturn)
{
	int numPeopleFound = 0;

	for (unsigned int i = 0; i < this->size(); i++)
	{
		sPerson* pPerson = static_cast<sPerson*>(this->_pMap[i]);

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

bool cDIYMap::SortPeople(std::vector<sPerson>& vecPeople, eSortType sortType)
{
	return false;
}

bool cDIYMap::GetPerformanceFromLastCall(sPerfData& callStats)
{
	return false;
}

eContainerType cDIYMap::getContainerType(void)
{
	return eContainerType::CUSTOM_DIY_MAP;
}

unsigned int cDIYMap::_GenerateHash(const std::string& index)
{
	// Generate hash based on the whole string, char by char
	unsigned int indexHashValue = 0;

	for (unsigned int i = 0; i != index.size(); i++)
	{
		char curChar = index[i];
		indexHashValue += (unsigned int)curChar;
	}

	indexHashValue = indexHashValue % this->_curSize;

	return indexHashValue;
}
