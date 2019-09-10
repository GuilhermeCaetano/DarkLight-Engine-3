// cDIYList.cpp

#include "cDIYList.h"
#include <fstream>



cDIYList::cDIYList()
{
	// Default size and Initial capacity
	this->_curCapacity = 100;
	this->_curSize = 0;
	this->_nextIndex = 0;

	this->_InitArray();

	return;
}

cDIYList::~cDIYList()
{
	return;
}

cDIYList::cNode::cNode()
{
	this->pNextNode = nullptr;
	this->pPriorNode = nullptr;
}

void cDIYList::_InitArray()
{
	// No nodes are known in the beggining
	this->_pRootNode = nullptr;
	this->_pCurrentNode = nullptr;
}

void cDIYList::push_back(void* data)
{
	// Check if the current node is null
	// if it is, this will be the current
	if (this->_pCurrentNode == nullptr)
	{
		cNode* pNewNode = new cNode();
		pNewNode->data = data;

		// Point the current node to this node
		this->_pCurrentNode = pNewNode;

		// Is this also the first node?
		if (this->_pRootNode == nullptr)
		{
			// Make it the root
			this->_pRootNode = pNewNode;
		}
	}
	else
	{	
		// There's already something at the "current" node
		cNode* pNewNode = new cNode();

		// Copy the information
		pNewNode->data = data;

		// Place this node AFTER the current node
		// By taking the current node's "next node" 
		// and pointing it to this new node.
		this->_pCurrentNode->pNextNode = pNewNode;

		// Doubly linked list.. points to prior node
		pNewNode->pPriorNode = this->_pCurrentNode;

		// Move the current node pointer
		this->_pCurrentNode = pNewNode;
	}
}

void cDIYList::MoveToStart(void)
{
	// Set back to root node
	this->_pCurrentNode = this->_pRootNode;
}

bool cDIYList::MoveToNext(void)
{
	// Move to the next node if any
	if (this->_pCurrentNode->pNextNode != nullptr)
	{
		// It's not NULL, so must be pointing to something
		this->_pCurrentNode = this->_pCurrentNode->pNextNode;
		return true;
	}

	return false;
}

void* cDIYList::at()
{
	return this->_pCurrentNode;
}

unsigned long cDIYList::size()
{
	return this->_curSize;
}

bool cDIYList::LoadDataFilesIntoContainer(std::string firstNameFemaleFileName, std::string firstNameMaleFileName, std::string lastNameFileName)
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
		this->push_back(person);
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
		this->push_back(person);
	}

	return true;
}

bool cDIYList::FindPeopleByName(std::vector<sPerson>& vecPeople, sPerson personToMatch, int maxNumberOfPeople)
{
	int numPeopleFound = 0;

	for (unsigned int i = 0; i < this->size(); i++)
	{
		// Get data at current node
		sPerson* person = static_cast<sPerson*>(this->at());

		if (person->first == personToMatch.first && person->last == personToMatch.last)
		{
			vecPeople.push_back(*person);
			numPeopleFound++;

			if (numPeopleFound >= maxNumberOfPeople)
			{
				return true;
			}
		}

		// Move to next node
		this->MoveToNext();
	}

	return numPeopleFound > 0;
}

bool cDIYList::FindPeopleByName(std::vector<sPerson>& vecPeople, std::vector<sPerson>& vecPeopleToMatch, int maxNumberOfPeople)
{
	int numPeopleFound = 0;

	for (unsigned int i = 0; i < this->size(); i++)
	{
		// Get data at current node
		sPerson* pPerson = static_cast<sPerson*>(this->at());
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

		// Move to next node
		this->MoveToNext();
	}

	return numPeopleFound > 0;
}

bool cDIYList::FindPersonByID(sPerson& person, unsigned long long uniqueID)
{
	for (unsigned int i = 0; i < this->size(); i++)
	{
		// Get data at current node
		sPerson* pPerson = static_cast<sPerson*>(this->at());

		if (pPerson->uniqueID == uniqueID)
		{
			person = *pPerson;
			return true;
		}

		// Move to next node
		this->MoveToNext();
	}

	return false;
}

bool cDIYList::FindPeople(std::vector<sPerson>& vecPeople, sPoint location, float radius, int maxPeopleToReturn)
{
	int numPeopleFound = 0;

	for (unsigned int i = 0; i < this->size(); i++)
	{
		// Get data at current node
		sPerson* pPerson = static_cast<sPerson*>(this->at());

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

		// Move to next node
		this->MoveToNext();
	}

	return numPeopleFound > 0;
}

bool cDIYList::FindPeople(std::vector<sPerson>& vecPeople, float minHealth, float maxHealth, int maxPeopleToReturn)
{
	int numPeopleFound = 0;

	for (unsigned int i = 0; i < this->size(); i++)
	{
		// Get data at current node
		sPerson* pPerson = static_cast<sPerson*>(this->at());

		if (pPerson->health >= minHealth && pPerson->health <= maxHealth)
		{
			vecPeople.push_back(*pPerson);
			numPeopleFound++;

			if (numPeopleFound >= maxPeopleToReturn)
			{
				return true;
			}
		}

		// Move to next node
		this->MoveToNext();
	}

	return numPeopleFound > 0;
}

bool cDIYList::FindPeople(std::vector<sPerson>& vecPeople, sPoint location, float radius, float minHealth, float maxHealth, int maxPeopleToReturn)
{
	int numPeopleFound = 0;

	for (unsigned int i = 0; i < this->size(); i++)
	{
		// Get data at current node
		sPerson* pPerson = static_cast<sPerson*>(this->at());

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

		// Move to next node
		this->MoveToNext();
	}

	return numPeopleFound > 0;
}

bool cDIYList::SortPeople(std::vector<sPerson>& vecPeople, eSortType sortType)
{
	return false;
}

bool cDIYList::GetPerformanceFromLastCall(sPerfData& callStats)
{
	return false;
}

eContainerType cDIYList::getContainerType(void)
{
	return eContainerType::CUSTOM_DIY_LIST;
}
