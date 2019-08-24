#ifndef _nECSMapper_HG_
#define _nECSMapper_HG_

#include <string>


namespace nECSMapper
{
	template <typename T>
	std::string GetNameTypeFromClass()
	{
		if (T == sEntity) { return "ENTITY"; };
		else if (T == sLocationComponent) { return "LOCATION_COMPONENT"; };
		else if (T == sCameraComponent) { return "CAMERA_COMPONENT"; };
		else if (T == sMeshComponent) { return "MESH_COMPONENT"; };
		else if (T == sSkinnedMeshComponent) { return "SKINNEDMESH_COMPONENT"; };
		else if (T == sLightComponent) { return "LIGHT_COMPONENT"; };
		else if (T == sAudioComponent) { return "AUDIO_COMPONENT"; };
		else if (T == sFlockComponent) { return "FLOCK_COMPONENT"; };
		else if (T == sPathFollowComponent) { return "PATHFOLLOW_COMPONENT"; };
		else if (T == sNeuralNetworkComponent) { return "NEURALNETWORK_COMPONENT"; };
		else if (T == sSimpleRigidbodyComponent) { return "RIGIDBODY_COMPONENT"; };
		else if (T == sSimpleSoftbodyComponent) { return "SOFTBODY_COMPONENT"; };
		else if (T == sSimpleCompoundbodyComponent) { return "COMPOUNDBODY_COMPONENT"; };
		else if (T == sBtRigidbodyComponent) { return "BTRIGIDBODY_COMPONENT"; };
		else if (T == sBtSoftbodyComponent) { return "BTSOFTBODY_COMPONENT"; };
		else if (T == sBtCompoundbodyComponent) { return "BTCOMPOUNDBODY_COMPONENT"; };
		else if (T == sBt3DPersonControllerComponent) { return "BT3DPERSONCONTROLLER_COMPONENT"; };
	}

	/*template <typename T>
	T GetClassFromName(const std::string& type)
	{
		if (type == "ENTITY") { return sEntity; };
		else if (type == "LOCATION_COMPONENT") { return sLocationComponent; };
		else if (type == "CAMERA_COMPONENT") { return sCameraComponent; };
		else if (type == "MESH_COMPONENT") { return sMeshComponent; };
		else if (type == "SKINNEDMESH_COMPONENT") { return sSkinnedMeshComponent; };
		else if (type == "LIGHT_COMPONENT") { return sLightComponent; };
		else if (type == "AUDIO_COMPONENT") { return sAudioComponent; };
		else if (type == "FLOCK_COMPONENT") { return sFlockComponent; };
		else if (type == "PATHFOLLOW_COMPONENT") { return sPathFollowComponent; };
		else if (type == "NEURALNETWORK_COMPONENT") { return sNeuralNetworkComponent; };
	}*/
};

#endif // !_nECSMapper_HG_

