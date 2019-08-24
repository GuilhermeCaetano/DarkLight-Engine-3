#ifndef _sNeuralNetworkComponent_HG_
#define _sNeuralNetworkComponent_HG_

#include <Entity\Components\iComponent.h>
#include <OpenGL\OpenGLHeaders.h>
#include <vector>


struct sNeuralNetworkComponent : public iComponent
{
public:
	sNeuralNetworkComponent(unsigned long long entityID);
	~sNeuralNetworkComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "NEURALNETWORK_COMPONENT"; };

	std::vector<float> GetWeights();
	void SetWeights(const std::vector<float>& vecWeights);

	void Kill();

	float maxDistance;
	bool bPathSet = false;

	std::vector<glm::vec3> detectorPoints;

protected:
	void _SaveLastWeights();
	void _LoadLastWeights();

	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

	std::vector<float> _vecWeights;
	std::vector<float> _vecSaveWeights;

	glm::vec3 initPosition;

};

#endif // !_sNeuralNetworkComponent_HG_

