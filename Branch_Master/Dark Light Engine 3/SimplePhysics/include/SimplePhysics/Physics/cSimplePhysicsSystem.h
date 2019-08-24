#ifndef _cSimplePhysicsSystem_HG_
#define _cSimplePhysicsSystem_HG_

#include <Interfaces\Physics\iPhysicsSystem.h>
#include <Interfaces\Physics\iRigidbody.h>
#include <Interfaces\Physics\iSoftbody.h>
#include <Interfaces\Physics\iShape.h>
#include <Interfaces\Physics\iSphereShape.h>
#include <vector>
#include <OpenGL\OpenGLHeaders.h>
#include <Interfaces\Collisions\iOnCollision.h>

#include <PhyMath\nPhyMath.h>

/*reflect v,n
		velocity = reflect velocity, n

			ncomp = proj(velocity, n)
			velocity -= comp * 0.2f
			rk4 dt * (1 - t)

		position  = c + v * t*/

namespace nPhysics
{
	struct sSimpleRigidbodyComponent;
	class cSimplePhysicsSystem : public iPhysicsSystem
	{
	public:
		cSimplePhysicsSystem();
		~cSimplePhysicsSystem();

		struct sPhysicsEntity
		{
			iEntity* entity = nullptr;
			iCollisionBody* body = nullptr;
			iCollisionBody* other = nullptr;
			sCollisionStats collisionInfo;
		};

		virtual void SetGravity(const glm::vec3& gravity);
		virtual void SetDebugRenderer(iDebugRenderer* pDebugRenderer);

		virtual bool ScheduleEntity(iEntity* entity, iCollisionBody* rigidbody);
		virtual bool UnscheduleEntity(iEntity* entity, iCollisionBody* rigidbody);

		virtual void InitializeSystem();
		virtual void UpdateSystem(double deltaTime);

		virtual bool OnCollisionEnter(const sCollisionStats& info);

		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	private:
		struct sState
		{
			glm::vec3 x;
			glm::vec3 v;
		};

		struct sDerivatives
		{
			glm::vec3 dx = glm::vec3(0.0f);
			glm::vec3 dv = glm::vec3(0.0f);
		};

		void EulerCromerIntegration(sPhysicsEntity* entity, double deltaTime);

		void RK4Integration(sPhysicsEntity* entity, const sState& state, glm::vec3 acceleration, double deltaTime);
		sDerivatives RK4EvaluateDerivative(const sState& state, glm::vec3 acceleration, double deltaTime, sDerivatives& derivative);

		glm::vec3 CalculateDrag(sSimpleRigidbodyComponent* rb, float rbDrag);
		void ClampAccelAndVel(sSimpleRigidbodyComponent* rb);

		int _CollisionStatus(sPhysicsEntity* entityA, glm::vec3 moveVectorA, sPhysicsEntity* entityB, glm::vec3 moveVectorB, sCollisionStats& collisionStatA, sCollisionStats& collisionStatB);

		int _CheckPlaneSphereCollision(iCollisionBody* sphere, glm::vec3 moveVector, iCollisionBody* plane, float& time, glm::vec3& collisionPoint);
		int _CheckSphereSphereCollision(sPhysicsEntity* sphere0, sPhysicsEntity* sphere1, glm::vec3 moveVector0, glm::vec3 moveVector1, float& contactTime, glm::vec3& collisionPoint);
		int _CheckSphereSphereCollision(iCollisionBody* bodyA, iSphereShape* sphere0, iCollisionBody* bodyB, iSphereShape* sphere1);
		int _CheckSphereSphereCollision(iCollisionBody* bodyA, iSphereShape* sphere0, iCollisionBody* bodyB, iSphereShape* sphere1, unsigned int indexB = 0, unsigned int indexA = 0);

		void _CollideRigidSoftBody(iRigidbody* rigidbody, iSoftbody* softbody);

		void SimpleCollisionReaction(sPhysicsEntity* physicsEntity, double deltaTime);

		glm::vec3 _ClosestPointPlane(const glm::vec3& point, const glm::vec3& n, float d);

		
		//void _ControlWind(cIOManager* pIOManager);
		static cSimplePhysicsSystem* callbackPointer;

		int _controllingEntity;

		bool previousPress;
		glm::vec3 _gravity;
		//std::vector<sPhysicsEntities*> _vecEntities;

		std::vector<sPhysicsEntity*> _vecRigidbodies;
		std::vector<sPhysicsEntity*> _vecSoftbodies;
	};
}


#endif // !_cSimplePhysicsSystem_HG_
