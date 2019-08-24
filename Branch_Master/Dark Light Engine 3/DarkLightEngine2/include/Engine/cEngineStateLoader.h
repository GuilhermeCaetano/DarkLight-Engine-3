#ifndef _cEngineStateLoader_HG_
#define _cEngineStateLoader_HG_

#include <Interfaces\Physics\iPhysicsFactory.h>
#include <string>

struct sEntity;
struct iComponent;
class cEntityManager;
class cVaoMeshManager;
class cAssimpMeshLoader;


class cEngineStateLoader
{
public:
	cEngineStateLoader();
	~cEngineStateLoader();

	// Sets the name of the .json file to be loaded
	void SetEngineStateSaveName(const std::string& stateFilename);
	bool CheckEngineStateExists(const std::string& stateFilename);

	// Save and Load the last state of the engine from a .json file
	bool SaveEngineState_Json();
	bool LoadEngineState_Json(cEntityManager* pEntityManager, nPhysics::iPhysicsFactory* pPhysicsFactory);

private:
	//TODO: add load for configuration here too?
	//bool _LoadConfiguration();
	bool _LoadEntities();
	bool _LoadComponents(sEntity* entity, rapidjson::Value::ConstValueIterator entityIter);

	bool _LoadModelSpecifics(iComponent* component, rapidjson::Value::ConstValueIterator entityIter);
	bool _LoadPhysicsSpecifics(sEntity* entity, iComponent* component);

	std::string _stateFilename;

	// Temporary pointers that will be used when loading
	cEntityManager* _pEntityManager;
	cVaoMeshManager* _pVaoMeshManager;
	cAssimpMeshLoader* _pAssimpLoader;
	nPhysics::iPhysicsFactory* _pPhysicsFactory;

};


#endif // !_cEngineStateLoader_HG_

