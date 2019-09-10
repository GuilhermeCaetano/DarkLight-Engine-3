#ifndef _cTestScene_HG_
#define _cTestScene_HG_

#include <Scene\iScene.h>
#include <OpenGL\OpenGLHeaders.h>
#include <Interfaces\Physics\iPhysicsFactory.h>
#include <Entity\sEntity.h>
#include <vector>

class cVaoMeshManager;
class cEntityManager;
class cAssimpMeshLoader;


class cTestScene : public iScene
{
public:
	cTestScene(unsigned long long sceneID, std::string sceneName);
	virtual ~cTestScene();

	virtual unsigned long long GetSceneID();
	virtual std::string GetSceneName();

	// Creates all the objects needed for this scene.
	virtual void CreateScene(cVaoMeshManager* pVaoMeshManager, cEntityManager* pEntityManager, cAssimpMeshLoader* pAssimpLoader);

	// Begin controlling the scene. Awake is called before Start. 
	virtual void AwakeScene();
	virtual void StartScene(cEntityManager* pEntityManager);

	// Update and end the scene, cleaning everything up.
	virtual void UpdateScene();
	virtual void EndScene();

private:
	// Creates specific objects for the scene.
	virtual void _CreateObjects();
	virtual void _CreatePhysics();

	virtual sEntity* GetEntityByName(const std::string& entityName);

	unsigned long long _sceneID;
	std::string _sceneName;
	eSceneStates _sceneState;

	std::vector<sEntity*> _pSceneEntities;

};


#endif // !_cTestScene_HG_

