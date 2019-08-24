#ifndef _cDijkstra_HG_
#define _cDijkstra_HG_

#include <Scene\sPathNode.h>
#include <OpenGL\OpenGLHeaders.h>
#include <vector>
#include <deque>
#include <list>


class cDijkstra
{
public:
	cDijkstra();
	~cDijkstra();

	// Set start and goal nodes of the path
	void SetStartNode(sPathNode* node);
	void SetStartNode(const glm::vec3& position);
	void SetGoalNode(sPathNode* node);

	// Generate a new node and link them
	sPathNode* CreateNode(std::string nodeName, glm::vec3 nodePosition, int nodeDifficulty, bool isStartNode = false);

	// Get a node at the index
	sPathNode* GetNodeAt(int index);
	unsigned int GetNumNodes();

	// Find all neighbours and calculate scores
	void InitializeNodes(int rows, int columns, bool findNeighbours);

	// Make the path
	bool CalculatePath();
	bool MakePath(std::vector<sPathNode*>& path);
	bool MakePath(std::deque<sPathNode*>& path);
	bool DrawPath(class cDrawSystem* pDrawSystem, GLuint shaderProgramID);

	std::string goalName;

private:
	enum eDirections
	{
		TOP = 0,
		RIGHT = 1,
		BOTTOM = 2,
		LEFT = 3,
		TOPRIGHT = 4,
		BOTTOMRIGHT = 5,
		BOTTOMLEFT = 6,
		TOPLEFT = 7,
		INVALID = -1
	};

	void _CalculateScore(sPathNode* node);
	void _FindAllNeighbours(unsigned int index);

	sPathNode* _GetClosestOpenNode();
	sPathNode* _GetClosestOpenNode(const std::list<sPathNode*>& checkList);
	bool _IsNodeClosed(sPathNode* node);

	sPathNode* startNode;
	sPathNode* goalNode;
	

	int numRows;
	int numColumns;

	std::vector<std::pair<float/*score*/, sPathNode*/*node*/>> _vecNodes;

	// Sets
	std::list<sPathNode*> openSet;
	std::list<sPathNode*> closedSet;

	std::vector<sPathNode*> _vecPath;
	std::deque<sPathNode*> _deqPath;
};



#endif // !_cDijkstra_HG_

