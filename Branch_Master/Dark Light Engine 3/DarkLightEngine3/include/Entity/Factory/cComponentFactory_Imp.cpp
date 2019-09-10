// cComponentFactory_Imp.cpp

#include "cComponentFactory_Imp.h"
#include <Engine\ComponentHeaders.h>
#include <Engine\cECSPoolContainer.h>


cComponentFactory_Imp::cComponentFactory_Imp()
{
	return;
}

cComponentFactory_Imp::~cComponentFactory_Imp()
{
	return;
}

iComponent* cComponentFactory_Imp::CreateComponentOfType(const std::string& type, unsigned long long entityID)
{
	iComponent* newComponent = nullptr;
	void* allocElement = cECSPoolContainer::GetInstance()->InitPoolElement(type);
	if (type == "LOCATION_COMPONENT") { newComponent = ::new (allocElement) sLocationComponent(entityID); }
	else if (type == "CAMERA_COMPONENT") { newComponent = ::new (allocElement) sCameraComponent(entityID); }
	else if (type == "MESH_COMPONENT") { newComponent = ::new (allocElement) sMeshComponent(entityID); }
	else if (type == "SKINNEDMESH_COMPONENT") { newComponent = ::new (allocElement) sSkinnedMeshComponent(entityID); }
	else if (type == "LIGHT_COMPONENT") { newComponent = ::new (allocElement) sLightComponent(entityID); }
	else if (type == "AUDIO_COMPONENT") { newComponent = ::new (allocElement) sAudioComponent(entityID); }

	return newComponent;
}
