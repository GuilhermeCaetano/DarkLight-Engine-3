// sPointsColliderComponent.cpp

#include <Entity\Components\sPointsColliderComponent.h>


sPointsColliderComponent::sPointsColliderComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->Start();

	return;
}

sPointsColliderComponent::~sPointsColliderComponent()
{
	return;
}

void sPointsColliderComponent::Start()
{
	/*this->AddColliderPoint(glm::vec3(0.0f, 0.05f, 2.45f));
	this->AddColliderPoint(glm::vec3(1.58f, 1.0f, -1.79f));
	this->AddColliderPoint(glm::vec3(-1.58f, 1.0f, -1.79f));
	this->AddColliderPoint(glm::vec3(0.15f, -2.0f, 0.0f));*/
}

void sPointsColliderComponent::AddColliderPoint(glm::vec3 modelColliderPoint)
{
	this->_modelColliderPoints.push_back(modelColliderPoint);
}

std::vector<glm::vec3> sPointsColliderComponent::GetModelColliderPoints()
{
	return this->_modelColliderPoints;
}

std::vector<glm::vec3>& sPointsColliderComponent::GetFinalColliderPoints()
{
	return this->_colliderPoints;
}

void sPointsColliderComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{

}

void sPointsColliderComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator iter)
{

}
