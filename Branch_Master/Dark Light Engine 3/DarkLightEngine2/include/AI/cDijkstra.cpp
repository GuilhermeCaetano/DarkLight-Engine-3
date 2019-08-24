// cDijkstra.cpp

#include <AI\cDijkstra.h>
#include <algorithm>
#include <Pools\cPoolManager.h>
#include <Entity\Systems\cDrawSystem.h>
#include <Entity\Components\sMeshComponent.h>


cDijkstra::cDijkstra()
{
	return;
}

cDijkstra::~cDijkstra()
{
	std::list<sPathNode*>::iterator openIter = this->openSet.begin();
	std::list<sPathNode*>::iterator closedIter = this->closedSet.begin();

	for (openIter; openIter != this->openSet.end(); openIter++)
	{
		delete *openIter;
		*openIter = nullptr;
	}

	for (closedIter; closedIter != this->closedSet.end(); closedIter++)
	{
		delete *closedIter;
		*closedIter = nullptr;
	}

	return;
}

//sPathNode::sPathNode(std::string nodeName, glm::vec3 nodePosition, int nodeDifficulty, bool isStartNode = false)
//{
//	this->position = nodePosition;
//	this->travelDifficulty = nodeDifficulty;
//	this->distance = std::numeric_limits<float>::max();
//	this->optimalNode = nullptr;
//	this->vecNeighbours.resize(8);
//}

void cDijkstra::SetStartNode(sPathNode* node)
{
	this->startNode = node;
	node->_score = 0.0f;
}

void cDijkstra::SetStartNode(const glm::vec3& position)
{
	sPathNode* newStartNode = nullptr;
	float currentDistance = std::numeric_limits<float>::max();
	for (unsigned int i = 0; i < this->_vecNodes.size(); i++)
	{
		float distance = fabs(glm::distance(this->_vecNodes[i].second->_position, position));
		if (distance < currentDistance)
		{
			newStartNode = this->_vecNodes[i].second;
			currentDistance = distance;
		}
	}

	this->SetStartNode(newStartNode);
}

void cDijkstra::SetGoalNode(sPathNode* node)
{
	//this->goalNode = node;
}

sPathNode* cDijkstra::CreateNode(std::string nodeName, glm::vec3 nodePosition, int nodeDifficulty, bool isStartNode)
{
	sPathNode* newNode = new sPathNode(nodeName, nodePosition, nodeDifficulty, isStartNode);
	this->_vecNodes.push_back(std::make_pair(0.0f, newNode));
	return newNode;
}

sPathNode* cDijkstra::GetNodeAt(int index)
{
	return this->_vecNodes[index].second;
}

unsigned int cDijkstra::GetNumNodes()
{
	return this->_vecNodes.size();
}

void cDijkstra::InitializeNodes(int rows, int columns, bool findNeighbours)
{
	this->numRows = rows;
	this->numColumns = columns;

	for (unsigned int i = 0; i < this->_vecNodes.size(); i++)
	{
		sPathNode* node = this->_vecNodes[i].second;

		// Calculate Reset score and neighbours
		this->_vecNodes[i].second->_score = std::numeric_limits<float>::max();
		this->_vecNodes[i].second->_fScore = std::numeric_limits<float>::max();
		this->_vecNodes[i].second->_optimalNode = nullptr;

		if (this->_vecNodes[i].second == this->startNode)
		{
			this->_vecNodes[i].second->_score = 0.0f;
		}

		if (findNeighbours)
		{
			this->_FindAllNeighbours(i);
		}
	}
}

bool cDijkstra::CalculatePath()
{
	// Set initial node on open set
	//this->openSet.push_back(this->startNode);

	// Init from all nodes
	std::list<sPathNode*> tempNodes;
	for (unsigned int i = 0; i < this->_vecNodes.size(); i++)
	{
		tempNodes.push_back(this->_vecNodes[i].second);
	}

	// Start node already added and removed
	//this->currentNode = this->startNode;
	/*this->_vecPath.push_back(this->startNode);
	tempNodes.remove(this->startNode);
	this->closedSet.push_back(this->startNode);*/

	// Go through all the nodes
	while (tempNodes.size() > 0)
	{
		// Get closest node to start and remove from set
		sPathNode* closestNode = this->_GetClosestOpenNode(tempNodes);
		if (closestNode == nullptr)
		{
			break;
		}

		tempNodes.remove(closestNode);
		//this->openSet.remove(closestNode);
		this->closedSet.push_back(closestNode);

		if (closestNode->_travelDifficulty > 1)
		{
			continue;
		}

		//float currentDistance = std::numeric_limits<float>::max();
		// Check neighbours for new closest that is not already closed
		sPathNode* newCloserNode = nullptr;
		for (int i = 0; i < closestNode->vecNeighbours.size(); i++)
		{
			sPathNode* neighbourNode = closestNode->vecNeighbours[i];
			if (neighbourNode == nullptr || this->_IsNodeClosed(neighbourNode) || neighbourNode->_travelDifficulty > 1)
			{
				continue;
			}

			// Add neighbours to open set
			//this->openSet.push_back(neighbourNode);

			// Check distance and add to path if closest distance
			float distance = closestNode->_score + fabs(glm::distance(closestNode->_position, neighbourNode->_position));
			if (distance < neighbourNode->_score)
			{
				neighbourNode->_score = distance;
				neighbourNode->_optimalNode = closestNode;
			}
		}
	}

	this->goalNode = nullptr;
	float currentDistance = std::numeric_limits<float>::max();
	for (unsigned int i = 0; i < this->_vecNodes.size(); i++)
	{
		if (this->_vecNodes[i].second->_nodeName == this->goalName && this->_vecNodes[i].second->_score < currentDistance && this->_vecNodes[i].second->bIsActive)
		{
			this->goalNode = this->_vecNodes[i].second;
			currentDistance = goalNode->_score;
		}
	}

	return this->goalNode != nullptr;
}

bool cDijkstra::MakePath(std::vector<sPathNode*>& path)
{
	bool result = this->CalculatePath();

	if (!result)
	{
		return false;
	}

	this->openSet.clear();
	this->closedSet.clear();

	sPathNode* target = this->goalNode;
	while (target != nullptr)
	{
		this->_vecPath.push_back(target);
		target = target->_optimalNode;
	}

	path = this->_vecPath;

	return true;
}

bool cDijkstra::MakePath(std::deque<sPathNode*>& path)
{
	bool result = this->CalculatePath();

	if (!result)
	{
		return false;
	}

	this->openSet.clear();
	this->closedSet.clear();
	this->_deqPath.clear();

	sPathNode* target = this->goalNode;
	while (target != nullptr)
	{
		this->_deqPath.push_front(target);
		target = target->_optimalNode;
	}

	path = this->_deqPath;

	return true;
}

bool cDijkstra::DrawPath(cDrawSystem* pDrawSystem, GLuint shaderProgramID)
{
	if (this->_deqPath.size() == 0)
	{
		return false;
	}

	sEntity* debugSphere = cPoolManager::GetInstance()->GetEntityByName("DebugSphere");
	sMeshComponent* meshC = debugSphere->GetComponent<sMeshComponent>();

	for (unsigned int i = 0; i < this->_deqPath.size(); i++)
	{
		meshC->materialDiffuse = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		if (this->_deqPath[i] == this->startNode)
		{
			meshC->materialDiffuse = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		else if (this->_deqPath[i] == this->goalNode)
		{
			meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		}
		
		meshC->bIsVisible = true;
		meshC->nonUniformScale = glm::vec3(2.0f);
		debugSphere->transform->GetWorldPosition() = this->_deqPath[i]->_position;
		pDrawSystem->DrawCall(shaderProgramID, debugSphere, this->_deqPath[i]->_position, 0.0f);
		meshC->materialDiffuse = glm::vec4(1.0f);
		meshC->bIsVisible = false;
		meshC->nonUniformScale = glm::vec3(1.0f);
	}
}

void cDijkstra::_CalculateScore(sPathNode* node)
{
	float score = fabs(glm::distance(this->startNode->_position, node->_position));
	node->_score = score;
}

void cDijkstra::_FindAllNeighbours(unsigned int index)
{
	sPathNode* node = this->_vecNodes[index].second;

	// find neighbours
	int top = (index >= this->numColumns) ? (index - this->numColumns) : INVALID;
	int right = (index % this->numColumns) < (this->numColumns - 1) ? (index + 1) : INVALID;
	int bottom = (index + this->numColumns) < this->_vecNodes.size() ? (index + this->numColumns) : INVALID;
	int left = (index % this->numColumns) != 0 ? (index - 1) : INVALID;

	int topRight = (top != INVALID && right != INVALID) ? (index - this->numColumns + 1) : INVALID;
	int bottomRight = (bottom != INVALID && right != INVALID) ? (index + this->numColumns + 1) : INVALID;
	int bottomLeft = (bottom != INVALID && left != INVALID) ? (index + this->numColumns - 1) : INVALID;
	int topLeft = (top != INVALID && left != INVALID) ? (index - this->numColumns - 1) : INVALID;

	node->vecNeighbours[TOP] = ((top != INVALID ? this->_vecNodes[top].second : nullptr));
	node->vecNeighbours[RIGHT] = ((right != INVALID ? this->_vecNodes[right].second : nullptr));
	node->vecNeighbours[BOTTOM] = ((bottom != INVALID ? this->_vecNodes[bottom].second : nullptr));
	node->vecNeighbours[LEFT] = ((left != INVALID ? this->_vecNodes[left].second : nullptr));

	node->vecNeighbours[TOPRIGHT] = ((topRight != INVALID ? this->_vecNodes[topRight].second : nullptr));
	node->vecNeighbours[BOTTOMRIGHT] = ((bottomRight != INVALID ? this->_vecNodes[bottomRight].second : nullptr));
	node->vecNeighbours[BOTTOMLEFT] = ((bottomLeft != INVALID ? this->_vecNodes[bottomLeft].second : nullptr));
	node->vecNeighbours[TOPLEFT] = ((topLeft != INVALID ? this->_vecNodes[topLeft].second : nullptr));
}

sPathNode* cDijkstra::_GetClosestOpenNode()
{
	sPathNode* closestNode = nullptr;
	float currentDistance = std::numeric_limits<float>::max();
	for (std::list<sPathNode*>::const_iterator iter = this->openSet.begin(); iter != this->openSet.end(); iter++)
	{
		sPathNode* node = *iter;
		if (node->_score < currentDistance)
		{
			closestNode = node;
			currentDistance = node->_score;
		}
	}

	return closestNode;
}

sPathNode* cDijkstra::_GetClosestOpenNode(const std::list<sPathNode*>& checkList)
{
	sPathNode* closestNode = nullptr;
	float currentDistance = std::numeric_limits<float>::max();
	for (std::list<sPathNode*>::const_iterator iter = checkList.begin(); iter != checkList.end(); iter++)
	{
		sPathNode* node = *iter;

		if (closestNode == nullptr)
		{
			closestNode = node;
			currentDistance = node->_score;
		}
		else if (node->_score < currentDistance)
		{
			closestNode = node;
			currentDistance = node->_score;
		}
	}

	return closestNode;
}

bool cDijkstra::_IsNodeClosed(sPathNode* node)
{
	for (std::list<sPathNode*>::iterator iter = this->closedSet.begin(); iter != this->closedSet.end(); iter++)
	{
		if (*iter == node)
		{
			return true;
		}
	}

	return false;
}


