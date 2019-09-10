#ifndef _cScene_HG_
#define _cScene_HG_

#include <Engine\cEntityManager.h>
#include <Mesh\cVaoMeshManager.h>
#include <Scene\sPathNode.h>
#include <deque>


class cScene
{
public:
	cScene(cVaoMeshManager* pVaoMeshManager, cEntityManager* pEntityManager);
	~cScene();

	void BeginScene(unsigned int index);

	void UpdateScene(class cDrawSystem* pDrawSystem, GLuint shaderPorgramID, double deltaTime);

	void EndScene();

private:
	enum eAgentStates
	{
		IDLE,
		SEARCH,
		GATHER,
		RETURN,
		DONE
	};

	enum ePathTypes
	{
		DIJKSTRA,
		ASTAR
	};

	void _BeginDijkstraAStarScene();
	void _BeginNeuralNetworkScene();

	void _CreateAgentAt(glm::vec3 pos, float nodeSize);
	void _CreateBlockAt(glm::vec3 pos, float nodeSize);
	sEntity* _CreateResourceAt(glm::vec3 pos, float nodeSize);
	void _CreateNeuralCarAt(glm::vec3 pos, float nodeSize);
	void _CreateBaseAt(glm::vec3 pos, float nodeSize);
	void _DrawPath(class cDrawSystem* pDrawSystem, GLuint shaderPorgramID);

	cVaoMeshManager* _pVaoMeshManager;
	cEntityManager* _pEntityManager;

	class cDijkstra* _pDijkstra;
	class cAStar* _pAStar;
	std::vector<sPathNode*> _vecDjNodes;
	std::vector<sPathNode*> _vecAsNodes;

	std::deque<sPathNode*> _deqDjNodes;
	std::deque<sPathNode*> _deqAsNodes;

	sEntity* agent;
	eAgentStates _agentState;
	ePathTypes _pathType;
	unsigned int agentNodeIndex;
	float gatherTime;
	float baseTime;

	float _height;
	float _width;

};



#endif // !1
