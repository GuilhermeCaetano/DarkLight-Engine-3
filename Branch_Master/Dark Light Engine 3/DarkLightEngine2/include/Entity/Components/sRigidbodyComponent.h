#ifndef _sRigidbodyComponent_HG_
#define _sRigidbodyComponent_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Entity\Components\iComponent.h>


struct sRigidbodyComponent : iComponent
{
public:
	sRigidbodyComponent();
	~sRigidbodyComponent();

	virtual void _SerializeComponentToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeComponentFromJson(rapidjson::Value::ConstValueIterator iter);

	std::string physicsType;

	glm::vec3 acceleration;
	glm::vec3 deceleration;
	glm::vec3 velocity;

	glm::vec2 minMaxAcceleration;
	glm::vec2 minMaxVelocity;

	glm::vec3 angularAcceleration;
	glm::vec3 angularVelocity;

	glm::vec2 minMaxAngularAcceleration;
	glm::vec2 minMaxAngularVelocity;

	glm::vec3 gravityModifier;
	float mass;
	float invMass;
	float radius;
	float radiusAdjust;

	bool bUseGravity;
	bool bIsKinematic;

};



#endif // !_sRigidbodyComponent_HG_

