// sBulletSoftbodyComponent.cpp

#include <BulletPhysics\Physics\bullet\sBtSoftbodyComponent.h>
#include "sBtSoftbodyComponent.h"


namespace nPhysics
{
	sBtSoftbodyComponent::sBtSoftbodyComponent(const sSoftbodyDef& def, unsigned long long entityID)
	{
		return;
	}

	sBtSoftbodyComponent::~sBtSoftbodyComponent()
	{
		return;
	}

	glm::mat4 sBtSoftbodyComponent::GetTransform()
	{
		return glm::mat4();
	}

	void sBtSoftbodyComponent::SetTransform(const glm::mat4& transform)
	{
	}

	sLocationComponent* sBtSoftbodyComponent::GetTransformComponent()
	{
		return this->transform;
	}

	void sBtSoftbodyComponent::SetTransformComponent(sLocationComponent* locationC)
	{
		this->transform = locationC;
	}

	eCollisionLayerMask sBtSoftbodyComponent::GetCollisionLayerMask()
	{
		return this->_collisionLayerMask;
	}

	void sBtSoftbodyComponent::SetCollisionLayerMask(eCollisionLayerMask layerMask)
	{
		this->_collisionLayerMask = layerMask;
	}

	void sBtSoftbodyComponent::_GenerateSoftbody()
	{

	}

	unsigned int sBtSoftbodyComponent::GetNumNodes()
	{
		return 0;
	}

	glm::vec3 sBtSoftbodyComponent::GetNodePosition(unsigned int index)
	{
		return glm::vec3();
	}

	float sBtSoftbodyComponent::GetNodeRadius(unsigned int index)
	{
		return 0.0f;
	}

	void sBtSoftbodyComponent::GetAABB(glm::vec3 & minBoundsOut, glm::vec3 & maxBoundsOut)
	{

	}

	void sBtSoftbodyComponent::UpdateInternal(double deltaTime, const glm::vec3 & gravity)
	{

	}

	void sBtSoftbodyComponent::Cleanup()
	{

	}

	void sBtSoftbodyComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
	{

	}

	void sBtSoftbodyComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator iter)
	{

	}

	void sBtSoftbodyComponent::_CollideNodes(cNode * nodeA, cNode * nodeB, double deltaTime)
	{

	}

	void sBtSoftbodyComponent::_UpdateAABB()
	{

	}

	void sBtSoftbodyComponent::Start()
	{
	}
}
