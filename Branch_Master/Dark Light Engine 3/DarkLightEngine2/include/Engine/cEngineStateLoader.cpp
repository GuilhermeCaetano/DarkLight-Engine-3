// cEngineStateLoader.cpp

#include "cEngineStateLoader.h"
#include "cECSControl.h"
#include "cEntityManager.h"

#include <Loaders\nJsonLoader.h>
#include <rapidjson\document.h>
#include <rapidjson\filewritestream.h>
#include <rapidjson\prettywriter.h>

#include <Entity\Components\iComponent.h>
#include <Entity\Components\sMeshComponent.h>
#include <Pools\cPoolManager.h>


cEngineStateLoader::cEngineStateLoader()
{
	this->_stateFilename = "save.json";

	return;
}

cEngineStateLoader::~cEngineStateLoader()
{
	this->_pEntityManager = nullptr;
	this->_pVaoMeshManager = nullptr;
	this->_pAssimpLoader = nullptr;
	this->_pPhysicsFactory = nullptr;

	return;
}

void cEngineStateLoader::SetEngineStateSaveName(const std::string& saveFilename)
{
	this->_stateFilename = saveFilename;
}

bool cEngineStateLoader::CheckEngineStateExists(const std::string& stateFilename)
{
	return nLoad::FileExists(stateFilename.c_str());
}

bool cEngineStateLoader::SaveEngineState_Json()
{
	FILE* fp;
	errno_t error;

	error = fopen_s(&fp, this->_stateFilename.c_str(), "w");
	if (fp == nullptr)
	{
		//printf("Failed to open json file, error code: %d", error);
		return false;
	}

	char writeBuffer[65536];
	rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);

	writer.StartObject();
	writer.Key("Entities");
	writer.StartArray();

	cPoolManager* tempPool = cPoolManager::GetInstance();
	for (unsigned int i = 0; i != tempPool->vecStaticEntities.size(); i++)
	{
		sEntity* entity = tempPool->vecStaticEntities[i];
		if (entity->GetTag() == "NoSave")
		{
			continue;
		}
		writer.StartObject();
		entity->SerializeToJson(writer);
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();
	fclose(fp);

	return true;
}

bool cEngineStateLoader::LoadEngineState_Json(cEntityManager* pEntityManager, nPhysics::iPhysicsFactory* pPhysicsFactory)
{
	// Save temporary pointers to use with the functions
	this->_pEntityManager = pEntityManager;
	this->_pPhysicsFactory = pPhysicsFactory;

	// Load all the entities
	this->_LoadEntities();

	return true;
}

bool cEngineStateLoader::_LoadEntities()
{
	rapidjson::Document document;
	nLoad::LoadJsonFile(this->_stateFilename, document);

	// The DOM document in json is an object which will contain multiple arrays, each with their own parameters.
	// So first we go through the main DOM object.
	for (rapidjson::Value::ConstMemberIterator objectIter = document.MemberBegin(); objectIter != document.MemberEnd(); objectIter++)
	{
		if (objectIter->name == "Entities")
		{
			// Check each entity inside the main object
			for (rapidjson::Value::ConstValueIterator entityIter = objectIter->value.Begin(); entityIter != objectIter->value.End(); entityIter++)
			{
				// And finally, process the current array
				sEntity* newEntity = this->_pEntityManager->GenerateEntityOfType("EmptyPrefab");
				newEntity->DeserializeFromJson(entityIter);
				this->_LoadComponents(newEntity, entityIter);
			}
		}
	}

	return true;
}

bool cEngineStateLoader::_LoadComponents(sEntity* entity, rapidjson::Value::ConstValueIterator entityIter)
{
	// Look for components to load back on each entity
	for (rapidjson::Value::ConstMemberIterator mIter = entityIter->MemberBegin(); mIter != entityIter->MemberEnd(); mIter++)
	{
		if (mIter->name != "Components")
		{
			continue;
		}

		// Go through every component and load it
		for (rapidjson::Value::ConstValueIterator vIter = mIter->value.Begin(); vIter != mIter->value.End(); vIter++)
		{
			if (vIter->MemberCount() == 0)
			{
				continue;
			}

			if (vIter->MemberBegin()->name != "ComponentType")
			{
				continue;
			}

			std::string componentType = vIter->MemberBegin()->value.GetString();
			iComponent* newComponent = cECSControl::GetComponentManager()->CreateComponent(componentType, entity->GetInstanceID());
			
			// Add the component to the entity and deserialize it
			entity->AddComponent(newComponent);
			newComponent->DeserializeFromJson(vIter);

			// Load specifics
			if (componentType == "MESH_COMPONENT" || componentType == "SKINNEDMESH_COMPONENT")
			{
				this->_LoadModelSpecifics(newComponent, vIter);
			}
			else if (componentType == "RIGIDBODY_COMPONENT" || componentType == "BTRIGIDBODY_COMPONENT");
			{
				this->_LoadPhysicsSpecifics(entity, newComponent);
			}
			/*else if (componentType == LINE2LINE_COMPOUND_COMPONENT || componentType == FIXED_COMPOUND_COMPONENT
				|| componentType == SLIDER_COMPOUND_COMPONENT || componentType == HINGE_COMPOUND_COMPONENT)
			{
				nPhysics::iCompoundBody* cb = static_cast<nPhysics::iCompoundBody*>(newComponent);
				for (unsigned int i = 0; i < cb->_vecEntityIDs.size(); i++)
				{
					sEntity* entity = cPoolManager::GetInstance()->GetEntityByID(cb->_vecEntityIDs[i]);
					nPhysics::iRigidbody* rb = static_cast<nPhysics::iRigidbody*>(entity->GetComponent(RIGIDBODY_COMPONENT));
					cb->AddCompoundBody(rb, false);
				}
			}*/
		}
	}

	return true;
}

bool cEngineStateLoader::_LoadModelSpecifics(iComponent* component, rapidjson::Value::ConstValueIterator vIter)
{
	sMeshComponent* meshC = static_cast<sMeshComponent*>(component);
	sModelDrawInfo* meshInfo = meshC->meshInfo;

	if (meshInfo->skinnedModelInfo != nullptr)
	{
		meshC->SetAnimator();

		rapidjson::Value::ConstArray val = vIter->FindMember("Animations")->value.GetArray();
		unsigned int arraySize = val.Size();
		for (unsigned int i = 0; i != arraySize; i++)
		{
			meshC->animator->AddAnimation(val[i].GetString());
		}
	}

	return true;
}

bool cEngineStateLoader::_LoadPhysicsSpecifics(sEntity* entity, iComponent* component)
{
	nPhysics::iRigidbody* rb = static_cast<nPhysics::iRigidbody*>(component);
	sLocationComponent* transform = entity->transform;
	rb->SetTransformComponent(transform);
	if (rb->GetShapeType() == nPhysics::SHAPE_TYPE_BVH_TRIANGLE_MESH)
	{
		sMeshComponent* meshC = entity->GetComponent<sMeshComponent>();
		sModelDrawInfo* modelInfo = meshC->meshInfo;
		nPhysics::iBvhTriangleMeshShape* terrainShape = this->_pPhysicsFactory->CreateBvhTriangleMeshShape(modelInfo, true, true);

		rb->SetShape(terrainShape);
		rb->InitRigidbody();
	}

	return true;
}
