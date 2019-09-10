#ifndef _sPathNode_HG_
#define _sPathNode_HG_

#include <vector>
#include <OpenGL\OpenGLHeaders.h>
#include <Entity\sEntity.h>


struct sPathNode
{
public:
	sPathNode(std::string nodeName, glm::vec3 nodePosition, int nodeDifficulty, bool isStartNode = false)
	{
		// Set user defined variables
		this->bIsActive = true;
		this->_nodeName = nodeName;
		this->_position = nodePosition;
		this->_travelDifficulty = nodeDifficulty;

		// Set scores to max and optimal to null
		if (!isStartNode)
		{
			this->_score = std::numeric_limits<float>::max();
			this->_fScore = std::numeric_limits<float>::max();
		}
		else
		{
			this->_score = 0.0f;
			this->_fScore = 0.0f;
		}
		
		this->_optimalNode = nullptr;
		this->nodeEntity = nullptr;
		this->vecNeighbours.resize(8);
	}

	std::string _nodeName;
	glm::vec3 GetPosition() { return this->_position; };
	float GetScore() { return this->_score; };
	float GetFScore() { return this->_fScore; };
	int GetTravelDifficulty() { return this->_travelDifficulty; };
	sPathNode* GetOptimalNode() { return this->_optimalNode; };
	sEntity* nodeEntity;
	bool bIsActive;

	std::vector<sPathNode*> vecNeighbours;

	float _score; // distance from the start
	float _fScore; // distance to the goal passing through this node
	int _travelDifficulty;
	sPathNode* _optimalNode;
	glm::vec3 _position;
	
private:
	

};


#endif // !_sPathNode_HG_

