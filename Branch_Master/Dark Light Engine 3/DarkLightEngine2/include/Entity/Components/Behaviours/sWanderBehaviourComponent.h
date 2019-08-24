#ifndef _sWanderBehaviourComponent_HG_
#define _sWanderBehaviourComponent_HG_

#include <Entity\Components\Behaviours\iBehaviour.h>
#include <OpenGL\OpenGLHeaders.h>

struct sEntity;


struct sWanderBehaviourComponent : public iBehaviour
{
public:
	sWanderBehaviourComponent(unsigned long long entityID);
	~sWanderBehaviourComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "WANDERBEHAV_COMPONENT"; };

	sEntity* agent;

	glm::vec3 center;
	glm::vec3 wanderPoint;

	float radius;
	float minDistance;
	float speed;

protected:
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

};


#endif // !_sWanderBehaviourComponent_HG_
