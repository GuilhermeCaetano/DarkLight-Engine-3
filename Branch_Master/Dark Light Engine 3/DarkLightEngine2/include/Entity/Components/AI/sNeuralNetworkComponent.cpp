// sNeuralNetworkComponent.cpp

#include "sNeuralNetworkComponent.h"


sNeuralNetworkComponent::sNeuralNetworkComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->Start();

	return;
}

sNeuralNetworkComponent::~sNeuralNetworkComponent()
{
	return;
}

void sNeuralNetworkComponent::Start()
{
	this->maxDistance = 5.0f;
	this->bPathSet = false;

	for (unsigned int i = 0; i < 5; i++)
	{
		this->detectorPoints.push_back(glm::vec3(0.0f));
	}
}

std::vector<float> sNeuralNetworkComponent::GetWeights()
{
	return this->_vecWeights;
}

void sNeuralNetworkComponent::SetWeights(const std::vector<float>& vecWeights)
{
	this->_vecWeights = vecWeights;
	this->bPathSet = true;
}

void sNeuralNetworkComponent::Kill()
{
	this->bPathSet = false;
}

void sNeuralNetworkComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{

}

void sNeuralNetworkComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator iter)
{

}

void sNeuralNetworkComponent::_SaveLastWeights()
{

}

void sNeuralNetworkComponent::_LoadLastWeights()
{

}
