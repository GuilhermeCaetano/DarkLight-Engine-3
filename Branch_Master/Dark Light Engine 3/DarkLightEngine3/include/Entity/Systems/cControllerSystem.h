#ifndef _cControllerSystem_HG_
#define _cControllerSystem_HG_

#include "iSystem.h"
#include <OpenGL\OpenGLHeaders.h>
#include <Entity\sEntity.h>
#include <Interfaces\Physics\iRigidbody.h>

#include <vector>


class cIOManager;
class cDebugRenderer;
class cControllerSystem : public iSystem
{
public:
	cControllerSystem();
	~cControllerSystem();

	enum ControllerTypes
	{
		CONTROLLER_3DPERSON,
		CONTROLLER_FLYING
	};

	struct sControllerEntity
	{
		sEntity* entity;
		nPhysics::iRigidbody* rigidbodyC;
		struct i3DPersonController* tdController;
	};

	void SetupSystem(sEntity* worldCamera, cIOManager* pIOManager, cDebugRenderer* pDebugRenderer);
	void SetCamera(sEntity* worldCamera);
	void SetIO(cIOManager* pIOManager);
	void SetDebugRenderer(cDebugRenderer* pDebugRenderer);

	void SetControllerStatus(sEntity* entity, ControllerTypes controllerType, bool isControlling);

	// Updates the system
	virtual void UpdateSystem(double deltaTime);

	// Schedules and unschedule specific components
	// in order to save searching memory
	virtual bool ScheduleEntity(sEntity* entity, iComponent* component);
	virtual bool UnscheduleEntity(sEntity* entity, iComponent* component);

	sEntity* MoveToNextController();

	void MoveForward(float moveAmount);
	void MoveBackward(float moveAmount);
	void MoveRight(float moveAmount);
	void MoveLeft(float moveAmount);

	void RotateUp(float moveAmount);
	void RotateDown(float moveAmount);
	void RotateLeft(float moveAmount);
	void RotateRight(float moveAmount);
	void RotateBarrelLeft(float moveAmount);
	void RotateBarrelRight(float moveAmount);

	void FirePrimary(sEntity* entity);

	glm::vec3 GetDirection();


protected:
	virtual void _Cleanup();

private:
	void _UpdateAcceleration(float val);
	void _UpdateAngularAcceleration(float val);

	void _Update3DPersonController(sControllerEntity controllerEntity, double deltaTime);
	
	glm::vec3 _GetControllerStickDirection();
	void _StickToWorldSpace(glm::vec3& stickDirection);
	bool _CheckForGrounded(const glm::vec3& checkPos, const glm::vec3& direction);

	// Breaking the behaviour down
	// 3D Person:
	void _3DP_SimpleMovement();
	void _3DP_PhyKinematicMovement(const glm::vec3& stickDirection, double deltaTime);
	void _3DP_PhyKinematicJump(const glm::vec3& stickDirection, double deltaTime);

	cIOManager* _pIOManager;
	cDebugRenderer* _pDebugRenderer;
	struct sCameraComponent* _cameraC;
	sControllerEntity curControllerEntity;

	nPhysics::iRigidbody* _currentRigidbodyC;
	sEntity* _currentEntity;

	unsigned int _controllerIndex;
	std::vector<sControllerEntity*> _vecControllers;

	std::string footstepType;


	float curFootstepTime;
	float maxFootstepTime;

};


#endif // !_cControllerSystem_HG_

