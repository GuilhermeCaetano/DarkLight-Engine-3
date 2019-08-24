#ifndef _sSeekBehaviourComponent_HG_
#define _sSeekBehaviourComponent_HG_

#include <Entity\Components\Behaviours\iBehaviour.h>
#include <OpenGL\OpenGLHeaders.h>

struct sEntity;


struct sSeekBehaviourComponent : public iBehaviour
{
public:
	sSeekBehaviourComponent(unsigned long long entityID);
	~sSeekBehaviourComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "SEEKBEHAV_COMPONENT"; };

	eBehaviourTypes behaviourType;

	sEntity* agent;
	iComponent* target;

	glm::vec3 agentDirection;
	glm::vec3 targetDirection;
	glm::vec3 moveDirection;

	float distAgentTarget;
	float angleAgentTarget;

protected:
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

};


#endif // !_sSeekBehaviourComponent_HG_
