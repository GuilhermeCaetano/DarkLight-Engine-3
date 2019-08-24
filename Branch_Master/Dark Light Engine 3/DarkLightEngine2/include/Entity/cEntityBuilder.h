#ifndef _cEntityBuilder_HG_
#define _cEntityBuilder_HG_

#include <string>


struct sEntity;
class cEntityBuilder
{
public:
	cEntityBuilder();
	~cEntityBuilder();

	void BuildEntity(const std::string& prefabType, sEntity* entityToBuild);

private:
	void _BuildStaticPrefab(sEntity* prefabToBuild);
	void _BuildSkyboxPrefab(sEntity* prefabToBuild);
	void _BuildCameraPrefab(sEntity* prefabToBuild);
	void _BuildLightPrefab(sEntity* prefabToBuild);
	void _BuildRigidbodyPrefab(sEntity* prefabToBuild);
	void _BuildCompoundbodyPrefab(sEntity* prefabToBuild);
	void _BuildPhysicsTerrain(sEntity* prefabToBuild);
	void _BuildPlayerPrefab(sEntity* prefabToBuild);
	void _BuildPatrolEnemyPrefab(sEntity* prefabToBuild);

};


#endif // !_cEntityBuilder_HG_

