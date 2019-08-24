#ifndef _iScene_HG_
#define _iScene_HG_


class cVaoMeshManager;
class cEntityManager;
class cAssimpMeshLoader;


class iScene
{
public:
	virtual ~iScene() {};
	
	enum eSceneStates
	{
		UNLOADED,
		LOADING,
		LOADED
	};

	virtual unsigned long long GetSceneID() = 0;

	// Creates all the objects needed for this scene.
	virtual void CreateScene(cVaoMeshManager* pVaoMeshManager, cEntityManager* pEntityManager, cAssimpMeshLoader* pAssimpLoader) = 0;

	// Begin controlling the scene. Awake is called before Start. 
	virtual void AwakeScene() = 0;
	virtual void StartScene(cEntityManager* pEntityManager) = 0;

	// Update and end the scene, cleaning everything up.
	virtual void UpdateScene() = 0;
	virtual void EndScene() = 0;

protected:
	iScene() {};
	iScene(const iScene& other) {};

	// Creates specific objects for the scene.
	virtual void _CreateObjects() = 0;
	virtual void _CreatePhysics() = 0;

};


#endif // !_iScene_HG_

