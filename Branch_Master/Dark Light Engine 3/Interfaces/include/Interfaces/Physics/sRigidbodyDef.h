#ifndef _sRigidbodyDef_HG_
#define _sRigidbodyDef_HG_

#include "iPhysicsDef.h"
#include <Interfaces\Physics\iShape.h>
#include <OpenGL\OpenGLHeaders.h>
#include <string>


namespace nPhysics
{
	struct sRigidbodyDef : public iPhysicsDef
	{
		sRigidbodyDef()
		{
			this->defType = "Rigidbody";
		}

		~sRigidbodyDef() {};

		glm::vec3 initialPosition = glm::vec3(0.0f);
		glm::quat initialRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

		glm::vec3 acceleration = glm::vec3(0.0f);
		glm::vec3 velocity = glm::vec3(0.0f);

		float radius = 0.0f;
		glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
		float constant = 0.0f;

		glm::vec3 extent = glm::vec3(0.0f);
		float extentScalar = 0.0f;

		float mass = 1.0f;
		float bounciness = 0.5f;
		float drag = 1.0f;
		glm::vec3 centerOfMassOffset = glm::vec3(0.0f);
		glm::vec3 freezePosition = glm::vec3(false, false, false);
		glm::vec3 freezeRotation = glm::vec3(false, false, false);

		bool bUseGravity = true;
		bool bIsKinematic = false;

		iShape* collisionShape = nullptr;
	};
}

#endif // !_nRigidbodyDef_HG_
