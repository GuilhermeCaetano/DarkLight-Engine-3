// cAStar.cpp

#include <AI\cAStar.h>
#include <Pools\cPoolManager.h>
#include <Entity\Systems\cDrawSystem.h>
#include <Entity\Components\sMeshComponent.h>


cAStar::cAStar()
{
	return;
}

cAStar::~cAStar()
{
	return;
}

//sPathNode::sPathNode(glm::vec3 nodePosition, int nodeDifficulty)
//{
//	this->position = nodePosition;
//	this->score = std::numeric_limits<float>::max();
//	this->fScore = std::numeric_limits<float>::max();
//	this->travelDifficulty = nodeDifficulty;
//	this->optimalNode = nullptr;
//	this->vecNeighbours.resize(8);
//}

void cAStar::SetStartNode(sPathNode* node)
{
	this->startNode = node;
}

void cAStar::SetStartNode(const glm::vec3& position)
{
	sPathNode* newStart = nullptr;

	float currentDistance = std::numeric_limits<float>::max();
	for (unsigned int i = 0; i < this->_vecNodes.size(); i++)
	{
		float distance = fabs(glm::distance(this->_vecNodes[i].second->_position, position));
		if (distance < currentDistance)
		{
			currentDistance = distance;
			newStart = this->_vecNodes[i].second;
		}
	}

	this->SetStartNode(newStart);
}

void cAStar::SetGoalNode(sPathNode* node)
{
	this->goalNode = node;
}

void cAStar::SetGoalNode(std::string nodeName)
{
	sPathNode* newGoal = nullptr;

	for (unsigned int i = 0; i < this->_vecNodes.size(); i++)
	{
		if (this->_vecNodes[i].second->_nodeName == nodeName)
		{
			newGoal = this->_vecNodes[i].second;
			break;
		}
	}

	this->SetGoalNode(newGoal);
}

sPathNode* cAStar::CreateNode(std::string nodeName, glm::vec3 nodePosition, int nodeDifficulty, bool isStartNode)
{
	sPathNode* newNode = new sPathNode(nodeName, nodePosition, nodeDifficulty, isStartNode);
	this->_vecNodes.push_back(std::make_pair(0.0f, newNode));
	return newNode;
}

sPathNode* cAStar::GetNodeAt(int index)
{
	return this->_vecNodes[index].second;
}

unsigned int cAStar::GetNumNodes()
{
	return this->_vecNodes.size();
}

void cAStar::InitializeNodes(int rows, int columns, bool findNeighbours)
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

			// Euclidian euristic
			this->_vecNodes[i].second->_fScore = fabs(glm::distance(this->startNode->_position, this->goalNode->_position));
		}

		if (findNeighbours)
		{
			this->_FindAllNeighbours(i);
		}
	}
}

bool cAStar::CalculatePath()
{
	// Reset the sets
	this->openSet.clear();
	this->closedSet.clear();

	// Set start node on open set
	this->openSet.push_back(this->startNode);

	std::vector<sPathNode*> cameFrom;

	// Go through the open set
	while (this->openSet.size() > 0)
	{
		// Get current closest node still open
		sPathNode* closestNode = this->_GetClosestOpenNode();

		// Check if the closest is the goal node
		if (closestNode == this->goalNode)
		{
			return this->RemakePath(cameFrom, closestNode);
		}

		this->openSet.remove(closestNode);
		this->closedSet.push_back(closestNode);

		if (closestNode->_travelDifficulty > 1)
		{
			continue;
		}

		// Check neighbours for closest
		sPathNode* newCloserNode = nullptr;
		for (int i = 0; i < closestNode->vecNeighbours.size(); i++)
		{
			sPathNode* neighbourNode = closestNode->vecNeighbours[i];
			if (neighbourNode == nullptr || this->_IsNodeClosed(neighbourNode))
			{
				continue;
			}

			// Add neighbours to open set
			//this->openSet.push_back(neighbourNode);

			// Check distance and add to path if closest distance
			float distance = closestNode->_score + fabs(glm::distance(closestNode->_position, neighbourNode->_position));

			// If the neighbour is not on the open set, add it
			if (!this->_IsNodeOpen(neighbourNode))
			{
				this->openSet.push_back(neighbourNode);
			}
			else if (distance >= neighbourNode->_score)
			{
				continue;
			}

			// If it got here, it's a new best route
			cameFrom.push_back(closestNode);
			neighbourNode->_optimalNode = closestNode;
			neighbourNode->_score = distance;
			neighbourNode->_fScore = neighbourNode->_score + (fabs(glm::distance(neighbourNode->_position, this->goalNode->_position)));
		}
	}

	return true;
}

bool cAStar::MakePath(std::vector<sPathNode*>& path)
{
	this->CalculatePath();
	path = this->_vecPath;

	return true;
}

bool cAStar::MakePath(std::deque<sPathNode*>& path)
{
	this->CalculatePath();
	path = this->_deqPath;

	return true;
}

bool cAStar::DrawPath(cDrawSystem* pDrawSystem, GLuint shaderProgramID)
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
		meshC->bIsVisible = true;
		meshC->nonUniformScale = glm::vec3(2.0f);
		pDrawSystem->DrawCall(shaderProgramID, debugSphere, this->_deqPath[i]->_position, 0.0f);
		meshC->materialDiffuse = glm::vec4(1.0f);
		meshC->bIsVisible = false;
		meshC->nonUniformScale = glm::vec3(1.0f);
	}
}

bool cAStar::RemakePath(const std::vector<sPathNode*>& cameFrom, sPathNode* fromNode)
{
	this->_deqPath.clear();

	this->_deqPath.push_front(fromNode);

	while (fromNode != nullptr)
	{
		this->_deqPath.push_front(fromNode);
		fromNode = fromNode->_optimalNode;
	}

	return true;
}

void cAStar::_CalculateScore(sPathNode* node)
{
	float score = fabs(glm::distance(this->startNode->_position, node->_position));
	node->_score = score;
}

void cAStar::_FindAllNeighbours(unsigned int index)
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

sPathNode* cAStar::_GetClosestOpenNode()
{
	sPathNode* closestNode = nullptr;
	float currentDistance = std::numeric_limits<float>::max();
	for (std::list<sPathNode*>::const_iterator iter = this->openSet.begin(); iter != this->openSet.end(); iter++)
	{
		sPathNode* node = *iter;
		if (node->_fScore < currentDistance)
		{
			closestNode = node;
			currentDistance = node->_fScore;
		}
	}

	return closestNode;
}

bool cAStar::_IsNodeClosed(sPathNode* node)
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

bool cAStar::_IsNodeOpen(sPathNode * node)
{
	for (std::list<sPathNode*>::iterator iter = this->openSet.begin(); iter != this->openSet.end(); iter++)
	{
		if (*iter == node)
		{
			return true;
		}
	}

	return false;
}
