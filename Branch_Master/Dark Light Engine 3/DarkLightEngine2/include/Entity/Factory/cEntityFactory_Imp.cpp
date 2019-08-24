// cEntityFactory_Imp.cpp

#include <Entity\Factory\cEntityFactory_Imp.h>
#include <Entity\cEntityBuilder.h>
#include <Engine\nAllocatedMemoryFinder.h>
#include <Entity\sEntity.h>


cEntityFactory_Imp::cEntityFactory_Imp()
{
	this->_pEntityBuilder = new cEntityBuilder();

	return;
}

cEntityFactory_Imp::~cEntityFactory_Imp()
{
	delete this->_pEntityBuilder;

	return;
}

//bool cEntityFactory_Imp::_CheckPrefabType(const std::string& prefabType)
//{
//	if (prefabType == "EmptyPrefab" || prefabType == "SkyboxPrefab" || prefabType == "CameraPrefab" || prefabType == "StaticPrefab" ||
//		prefabType == "LightPrefab" || prefabType == "PhysicsPrefab" || prefabType == "PhysicsTerrainPrefab" || prefabType == "CompoundPhysicsPrefab" ||
//		prefabType == "PlayerPrefab" || prefabType == "PatrolEnemyPrefab" ||
//		prefabType == "BulletPrefab")
//	{
//		return true;
//	}
//	if (prefabType == "ClothPrefab" || prefabType == "Line2LinePrefab" || prefabType == "FixedPrefab" || prefabType == "SliderPrefab" || prefabType == "HingePrefab")
//	{
//		return true;
//	}
//	
//	printf("Error adding %s prefab! Unknown prefab type!", prefabType.c_str());
//	return false;
//}

sEntity* cEntityFactory_Imp::CreateEntity(const std::string& prefabType)
{
	// Find allocated memory for the entity. If none exist, it will automatically be created.
	// Use placement new to call constructor manually, since we only get the malloc memory.
	void* allocElement = nMemory::FindAllocatedMemory("ENTITY");
	sEntity* newEntity = ::new(allocElement) sEntity();

	// Build if this is a prefab
	if (prefabType != "EmptyPrefab")
	{
		this->_pEntityBuilder->BuildEntity(prefabType, newEntity);
	}

	return newEntity;
}
