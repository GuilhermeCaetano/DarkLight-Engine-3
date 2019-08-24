#ifndef _sApproachBehaviourComponent_HG_
#define _sApproachBehaviourComponent_HG_

#include <Entity\Components\Behaviours\iBehaviour.h>
#include <OpenGL\OpenGLHeaders.h>

struct sEntity;


struct sApproachBehaviourComponent : public iBehaviour
{
public:
	sApproachBehaviourComponent(unsigned long long entityID);
	~sApproachBehaviourComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "APPROACHBEHAV_COMPONENT"; };

	sEntity* agent;
	iComponent* target;

	glm::vec3 agentDirection;
	glm::vec3 targetDirection;
	glm::vec3 moveDirection;

	float distAgentTarget;
	float angleAgentTarget;

	float coolDown;

protected:
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

};


#endif // !_sApproachBehaviourComponent_HG_

