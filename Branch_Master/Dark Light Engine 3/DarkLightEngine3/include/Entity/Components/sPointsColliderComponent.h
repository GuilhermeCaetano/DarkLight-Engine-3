#ifndef _sPointsColliderComponent_HG_
#define _sPointsColliderComponent_HG_

#include <Entity\Components\iComponent.h>
#include <OpenGL\OpenGLHeaders.h>
#include <vector>


struct sPointsColliderComponent : public iComponent
{
public:
	sPointsColliderComponent(unsigned long long entityID);
	~sPointsColliderComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "POINTSCOLLIDER_COMPONENT"; };

	void AddColliderPoint(glm::vec3 modelColliderPoint);
	std::vector<glm::vec3> GetModelColliderPoints();
	std::vector<glm::vec3>& GetFinalColliderPoints();

protected:
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

	std::vector<glm::vec3> _modelColliderPoints;
	std::vector<glm::vec3> _colliderPoints;

};



#endif // !_sPointsColliderComponent_HG_

