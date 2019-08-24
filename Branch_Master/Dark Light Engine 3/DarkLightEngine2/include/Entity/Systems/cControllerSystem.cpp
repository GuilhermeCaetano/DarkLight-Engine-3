// cControllerSystem.cpp

#include <Entity\Systems\cControllerSystem.h>
#include <SimplePhysics\Physics\sSimpleRigidbodyComponent.h>
#include <BulletPhysics\Physics\bullet\sBtRigidbodyComponent.h>
#include <Entity\Components\sLocationComponent.h>
#include <Entity\Components\sMeshComponent.h>
#include <Entity\Components\sCameraComponent.h>
#include <Entity\Components\sAudioComponent.h>
#include <IO\cIOManager.h>
#include <Pools\cPoolManager.h>
#include <Math\nMathHelper.h>
#include <Helpers\nPhysicsHelper.h>
#include <Math\nQuaternionHelper.h>
#include <Interfaces\Controllers\i3DPersonController.h>


cControllerSystem::cControllerSystem()
{
	this->_currentRigidbodyC = nullptr;
	this->_currentEntity = nullptr;

	this->_controllerIndex = 0;

	this->curFootstepTime = 0.0f;
	this->maxFootstepTime = 0.5f;

	return;
}

cControllerSystem::~cControllerSystem()
{
	this->CleanUp();

	return;
}

void cControllerSystem::SetupSystem(sEntity* worldCamera, cIOManager* pIOManager, cDebugRenderer* pDebugRenderer)
{
	this->SetCamera(worldCamera);
	this->SetIO(pIOManager);
	this->SetDebugRenderer(pDebugRenderer);
}

void cControllerSystem::SetCamera(sEntity* worldCamera)
{
	this->_cameraC = worldCamera->GetComponent<sCameraComponent>();
}

void cControllerSystem::SetIO(cIOManager* pIOManager)
{
	this->_pIOManager = pIOManager;
}

void cControllerSystem::SetDebugRenderer(cDebugRenderer* pDebugRenderer)
{
	this->_pDebugRenderer = pDebugRenderer;
}

void cControllerSystem::UpdateSystem(double deltaTime)
{
	if (cIOManager::GetEditorState() != cIOManager::Free)
	{
		return;
	}

	cPoolManager* tempPool = cPoolManager::GetInstance();

	for (unsigned int i = 0; i != tempPool->vecStaticEntities.size(); i++)
	{
		sEntity* entity = tempPool->vecStaticEntities[i];
		i3DPersonController* tDPersonControllerC = entity->GetComponent<i3DPersonController>();
		nPhysics::iRigidbody* rigidbodyC = entity->GetComponent<nPhysics::iRigidbody>();

		// Set current managing entity
		sControllerEntity controllerEntity;
		controllerEntity.entity = entity;
		controllerEntity.tdController = tDPersonControllerC;
		controllerEntity.rigidbodyC = rigidbodyC;
		this->curControllerEntity = controllerEntity;

		if (tDPersonControllerC != nullptr && tDPersonControllerC->GetIsControllable())
		{
			this->_Update3DPersonController(controllerEntity, deltaTime);
		}
	}
}

bool cControllerSystem::ScheduleEntity(sEntity* entity, iComponent* component)
{
	sControllerEntity* newController = new sControllerEntity();
	//sFlightControllerComponent* controllerC = static_cast<sFlightControllerComponent*>(primaryComponent);
	//s3DPersonControllerComponent* controllerC = static_cast<s3DPersonControllerComponent*>(primaryComponent);
	i3DPersonController* controllerC = static_cast<i3DPersonController*>(component);

	newController->entity = entity;
	newController->tdController = controllerC;

	this->_vecControllers.push_back(newController);

	return true;
}

bool cControllerSystem::UnscheduleEntity(sEntity* entity, iComponent* component)
{
	return false;
}

sEntity* cControllerSystem::MoveToNextController()
{
	if (this->_vecControllers.size() > 0)
	{
		//sMeshComponent* mesh1 = static_cast<sMeshComponent*>(this->_vecControllers[this->_controllerIndex]->entity->GetComponent(eComponentTypes::MESH_COMPONENT));
		//mesh1->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		this->_vecControllers[this->_controllerIndex]->tdController->SetIsControllable(false);

		this->_controllerIndex++;
		if (this->_controllerIndex >= this->_vecControllers.size())
		{
			this->_controllerIndex = 0;
		}
		this->_vecControllers[this->_controllerIndex]->tdController->SetIsControllable(true);
		//mesh1 = static_cast<sMeshComponent*>(this->_vecControllers[this->_controllerIndex]->entity->GetComponent(eComponentTypes::MESH_COMPONENT));
		//mesh1->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

		return this->_vecControllers[this->_controllerIndex]->entity;
	}

	return nullptr;
}

void cControllerSystem::SetControllerStatus(sEntity* entity, cControllerSystem::ControllerTypes controllerType, bool isControlling)
{
	switch (controllerType)
	{
		case ControllerTypes::CONTROLLER_3DPERSON:
		{
			i3DPersonController* tDPersonControllerC = entity->GetComponent<i3DPersonController>();
			if (tDPersonControllerC != nullptr)
			{
				//tDPersonControllerC->SetIsControlling(isControlling);
			}
		}
		break;
		/*case ControllerTypes::CONTROLLER_FLYING:
		{
			sFlightControllerComponent* flyControllerC = static_cast<sFlightControllerComponent*>(entity->GetComponent(eComponentTypes::FLIGHTCONTROLLER_COMPONENT));
			if (flyControllerC != nullptr)
			{
				flyControllerC->SetIsControlling(isControlling);
			}
		}
		break;*/
	}
}

void cControllerSystem::_Update3DPersonController(sControllerEntity controllerEntity, double deltaTime)
{
	sEntity* camera = cPoolManager::GetInstance()->GetEntityByName("WorldCamera");
	sCameraComponent* cameraC = camera->GetComponent<sCameraComponent>();

	//s3DPersonControllerComponent* controllerC = static_cast<s3DPersonControllerComponent*>(controllerEntity.tdController);
	i3DPersonController* controllerC = static_cast<i3DPersonController*>(controllerEntity.tdController);
	sAudioComponent* audioC = controllerEntity.entity->GetComponent<sAudioComponent>();

	// Look for sockets to find bone nodes
	sMeshComponent* meshC = controllerEntity.entity->GetComponent<sMeshComponent>();

	glm::mat4 mat = glm::mat4x4(1.0f);
	

	glm::mat4 matHand = meshC->meshInfo->skinnedModelInfo->GetBoneByName("mixamorig:RightHand");
	glm::mat4 matInitRotation = glm::mat4(controllerEntity.rigidbodyC->GetRotation());
	glm::quat finalQuat = controllerEntity.rigidbodyC->GetRotation() * glm::quat(matHand);
	finalQuat = nQuaternionHelper::AdjOrientationEulerAngles(finalQuat, glm::vec3(90.0f, 0.0f, 0.0f), true);
	glm::mat4 matHandRotation = glm::mat4(finalQuat);
	//matInitRotation = matHandRotation * matInitRotation;

	glm::vec4 matHandLocation = matHand * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float modelScale = meshC->nonUniformScale.x;
	matHandLocation = modelScale * matHandLocation;


	glm::mat4 matRotation = glm::mat4(controllerEntity.rigidbodyC->GetRotation());
	matHandLocation = matRotation * matHandLocation;
	//matHandLocation = matHandRotation * matHandLocation;

	/*glm::mat4 matHandRotation = glm::mat4(glm::quat(matHand));
	matHandLocation = matHandRotation * matHandLocation;*/

	glm::vec3 jointOffset = glm::vec3(0.0f, 2.0f, 0.0f);
	matHandLocation += glm::vec4(controllerEntity.rigidbodyC->GetPosition() - (controllerEntity.entity->transform->GetUpVector() * controllerEntity.rigidbodyC->GetShapeOffset()), 0.0f);

	//controllerC->SetSocketObjectPosition(0, glm::vec3(matHandLocation), glm::quat(matHandRotation));
	controllerC->SetHandPosition(glm::vec3(matHandLocation), glm::quat(matHandRotation));
	//controllerC->SetSocketObjectTransform(0, matFinal);

	std::vector<std::string> vecStrings;
	meshC->meshInfo->skinnedModelInfo->GetListOfBoneIDandNames(vecStrings);

	bool sexybreakpointhere = true;

	if (!controllerC->GetIsControllable())
	{
		
		//meshC->animator->SetCurrentAnimation(controllerC->idleName);
		return;
	}

	//if (controllerC->GetIsGrouded() && cameraC->GetCameraFollowType() == sCameraComponent::eFollowTypes::SIMPLE)
	//{
	//	glm::quat landQuat = glm::quat();
	//	//controllerEntity.entity->transform->qOrientation = nQuaternionHelper::AdjOrientationEulerAngles(landQuat, glm::vec3(0.0f, 0.0f, 0.0f), true);
	//	cameraC->SetTarget(controllerEntity.entity, glm::vec3(0.0f, 30.0f, -30.0f), 5.0f, 20.0f);
	//	cameraC->SetLookAtOffset(glm::vec3(0.0f, 20.0f, 1.0f));
	//	cameraC->SetCameraFollowType(sCameraComponent::eFollowTypes::ZELDA);
	//}
	
	// Calculate movement
	// First, get the controller stick direction.
	glm::vec3 stickDirection = this->_GetControllerStickDirection();
	glm::vec3 saveDirection = stickDirection;

	// Must. Not. Normalize. 0 vector. Seriously.
	if (stickDirection != glm::vec3(0.0f))
	{
		stickDirection = glm::normalize(stickDirection);

		if (controllerC->GetIsGrouded())
		{
			if (this->curFootstepTime <= 0.0f)
			{
				int random = nMathHelper::GetRandInRange<int>(1, 6);
				std::string soundName = "SFX_Footsteps_";
				soundName.append(std::to_string(random));
				soundName.append(".wav");
				audioC->SetSoundToPlay(soundName);
				this->curFootstepTime = this->maxFootstepTime;
			}
			else
			{
				this->curFootstepTime -= (float)deltaTime;
				if (this->curFootstepTime <= 0.0f)
				{
					this->curFootstepTime = 0.0f;
				}
			}
		}
	}
	this->_StickToWorldSpace(stickDirection);
	controllerC->SetMoveDirection(stickDirection);
	controllerC->SetFlyX(saveDirection.z);
	controllerC->SetFlyY(saveDirection.x);

	if (this->_pIOManager->IsKeyDown(GLFW_KEY_SPACE))
	{
		glm::vec3 jumpDirection = stickDirection;
		jumpDirection.y = 100.0f;
		controllerC->SetJumpDirection(jumpDirection);
	}

	if (this->_pIOManager->IsKeyDown(GLFW_KEY_F))
	{
		if(controllerC->GetIsGrouded())
		{
			glm::vec3 flyDirection = stickDirection;
			flyDirection.y = 10.0f;
			controllerC->SetFlyDirection(flyDirection);

			/*cameraC->SetTarget(controllerEntity.entity, glm::vec3(0.0f, 0.0f, 30.0f), 5.0f, 20.0f);
			cameraC->SetLookAtOffset(glm::vec3(0.0f, 30.0f, 1.0f));
			cameraC->SetCameraFollowType(sCameraComponent::eFollowTypes::SIMPLE);*/
		}
	}

	if (this->_pIOManager->IsKeyDown(GLFW_KEY_R))
	{
		controllerC->DropHeldObject();
	}

	if (this->_pIOManager->IsKeyDown(GLFW_KEY_Z) && !controllerC->GetIsAttacking())
	{
		controllerC->SetAttack();
		sEntity* masterSword = cPoolManager::GetInstance()->GetEntityByName("MasterSword");
	}
	else if (!controllerC->GetIsAttacking())
	{
		sEntity* masterSword = cPoolManager::GetInstance()->GetEntityByName("MasterSword");
	}

	std::string curAnimation = controllerC->GetAnimationName();

	meshC->animator->SetCurrentAnimation(curAnimation);

	// Only calculate relation if needed
	//if (stickDirection != glm::vec3(0.0f))
	//{
	//	// Stick direction is in local space, we need to convert it
	//	// into world space by putting it in relation to the camera's rotation.
	//	this->_StickToWorldSpace(stickDirection);

	//	// Move character physically
	//	//this->_3DP_PhyKinematicMovement(stickDirection, deltaTime);
	//}

	// Calculate jump
	//this->_3DP_PhyKinematicJump(stickDirection, deltaTime);

	/*if (!controllerC->bUsePhysics)
	{
		
	}
	else
	{
		
	}*/
}

//void cControllerSystem::_UpdateFlyingController(sEntity* entity, GLFWwindow* window)
//{
//	sFlightControllerComponent* flyControllerC = static_cast<sFlightControllerComponent*>(entity->GetComponent(eComponentTypes::FLIGHTCONTROLLER_COMPONENT));
//	if (flyControllerC == nullptr)
//	{
//		return;
//	}
//
//	if (!flyControllerC->_bIsControlling)
//	{
//		return;
//	}
//
//	nPhysics::iRigidbody* rigidbodyC = static_cast<nPhysics::iRigidbody*>(entity->GetComponent(eComponentTypes::RIGIDBODY_COMPONENT));
//	if (rigidbodyC == nullptr)
//	{
//		return;
//	}
//
//	this->_currentRigidbodyC = rigidbodyC;
//	this->_currentEntity = entity;
//
//	if (flyControllerC->coolDown)
//	{
//		flyControllerC->currentCoolDown -= 0.1f;
//		if (flyControllerC->currentCoolDown <= 0.0f)
//		{
//			flyControllerC->currentCoolDown = 0.0f;
//			flyControllerC->coolDown = false;
//		}
//	}
//
//	if (glfwGetKey(window, GLFW_KEY_SPACE))
//	{
//		flyControllerC->currentAcceleration += flyControllerC->thrusterAcceleration;
//		if (flyControllerC->currentAcceleration >= flyControllerC->minMaxThrusterVelocity.y)
//		{
//			flyControllerC->currentAcceleration = flyControllerC->minMaxThrusterVelocity.y;
//		}
//
//		this->MoveForward(flyControllerC->currentAcceleration);
//	}
//	else
//	{
//		flyControllerC->currentAcceleration = 0.0f;
//	}
//
//	if (glfwGetKey(window, GLFW_KEY_C) && !flyControllerC->coolDown)
//	{
//		this->FirePrimary(entity);
//	}
//
//	/*if (glfwGetKey(window, GLFW_KEY_W))
//	{
//		this->RotateDown(rigidbodyC->minMaxAngularAcceleration.y, flyControllerC->deltaThrusterAcceleration);
//	}
//	else if (glfwGetKey(window, GLFW_KEY_S))
//	{
//		this->RotateUp(rigidbodyC->minMaxAngularAcceleration.x, flyControllerC->deltaThrusterAcceleration);
//	}
//	else if (!glfwGetKey(window, GLFW_KEY_W) && !glfwGetKey(window, GLFW_KEY_A))
//	{
//		rigidbodyC->angularAcceleration.x = 0.0f;
//	}*/
//	//if (glfwGetKey(window, GLFW_KEY_A))
//	//{
//	//	//this->RotateLeft(flyControllerC->currentAcceleration);
//	//	this->RotateLeft(50.0f);
//	//}
//	//else if (glfwGetKey(window, GLFW_KEY_D))
//	//{
//	//	//this->RotateRight(flyControllerC->currentAcceleration);
//	//	this->RotateRight(-50.0f);
//	//}
//	else if (!glfwGetKey(window, GLFW_KEY_A) && !glfwGetKey(window, GLFW_KEY_D))
//	{
//		//rigidbodyC->angularAcceleration.y = 0.0f;
//	}
//	//if (glfwGetKey(window, GLFW_KEY_Q))
//	//{
//	//	this->RotateBarrelLeft(rigidbodyC->minMaxAngularAcceleration.x, flyControllerC->deltaThrusterAcceleration);
//	//}
//	//if (glfwGetKey(window, GLFW_KEY_E))
//	//{
//	//	this->RotateBarrelRight(rigidbodyC->minMaxAngularAcceleration.y, flyControllerC->deltaThrusterAcceleration);
//	//}
//	//else if (!glfwGetKey(window, GLFW_KEY_Q) && !glfwGetKey(window, GLFW_KEY_E))
//	//{
//	//	rigidbodyC->angularAcceleration.z = 0.0f;
//	//}
//}

glm::vec3 cControllerSystem::_GetControllerStickDirection()
{
	// Controller stick begins not moved at all
	glm::vec3 stickDirection = glm::vec3(0.0f);

	// Check for horizontal and vertical stick movement
	float vertical = 0.0f;
	float horizontal = 0.0f;

	// Get vertical input
	if (this->_pIOManager->IsKeyDown(GLFW_KEY_W))
	{
		vertical = 1.0f;
	}
	else if (this->_pIOManager->IsKeyDown(GLFW_KEY_S))
	{
		vertical = -1.0f;
	}

	// Get horizontal input. OpenGL uses -Z as forwad, so invert the x axis
	if (this->_pIOManager->IsKeyDown(GLFW_KEY_A))
	{
		horizontal = 1.0f;
	}
	else if (this->_pIOManager->IsKeyDown(GLFW_KEY_D))
	{
		horizontal = -1.0f;
	}

	// Normalize final direction and return
	stickDirection = glm::vec3(horizontal, 0.0f, vertical);
	
	return stickDirection;
}

void cControllerSystem::_StickToWorldSpace(glm::vec3& stickDirection)
{
	// Don't do anything if the vector is zero
	if (stickDirection == glm::vec3(0.0f))
	{
		return;
	}


	//glm::vec3 forward = glm::normalize(this->_cameraC->_lookAt);
	//glm::vec3 norm = glm::vec3(0.0f, 1.0f, 0.0f);
	//// Plane projection: B = A - (dot(A, n))*n
	//glm::vec3 planeProjec = forward - (glm::dot(forward, norm)) * norm;




	// Transform stick direction from local to world space
	// by applying camera matrix rotation to it.
	stickDirection = nMathHelper::GetRelativeAxis(-stickDirection, this->_cameraC->qOrientation);

	// Stick direction will have camera's Y rotation.
	// We don't want it, so kill it and re normalize the vector.
	stickDirection.y = 0;
	stickDirection = glm::normalize(stickDirection);
}

bool cControllerSystem::_CheckForGrounded(const glm::vec3& checkPos, const glm::vec3& direction)
{
	//TODO: this thing
	//this->_pDebugRenderer->addLine(checkPos, checkPos + direction * 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));


	return false;
}

// Rigidbody is kinematic, so we move the character manually, updating the physics counterpart for collision.
void cControllerSystem::_3DP_PhyKinematicMovement(const glm::vec3& stickDirection, double deltaTime)
{
	// Get used params for ease of use
	//sEntity* entity = this->curControllerEntity.entity;
	//nPhysics::iRigidbody* rb = this->curControllerEntity.rigidbodyC;
	//s3DPersonControllerComponent* controller = this->curControllerEntity.tdController;
	//s3DPersonControllerComponent::sJumpState* jumpState = controller->GetJumpState();

	//// Calculate new position based on stick direction.
	//glm::vec3 newPos = entity->transform->position + stickDirection * controller->GetMaxMoveSpeed() * 20.0f * (float)deltaTime;

	//glm::quat finalRotation = entity->transform->qOrientation;
	//if (jumpState->_state == s3DPersonControllerComponent::sJumpState::GROUNDED)
	//{
	//	// Calculate interpolated rotation based on stick direction
	//	glm::quat newRot = glm::quatLookAt(-stickDirection, glm::vec3(0.0f, 1.0f, 0.0f));

	//	float angle = glm::acos(glm::min(1.0f, glm::dot(entity->transform->direction, stickDirection)));
	//	float angleDeg = glm::degrees(angle);
	//	if (angleDeg > 1.0f)
	//	{
	//		float timeElapsed = 1.0f / angle;

	//		// Call Slerp
	//		finalRotation = glm::slerp(entity->transform->qOrientation, newRot, 0.1f * controller->GetMaxAngularSpeed());
	//	}
	//}

	//// Move and rotate entity
	//entity->transform->position = newPos;
	//entity->transform->qOrientation = finalRotation;

	//// Update physics counterpart
	//rb->SetPosition(newPos + rb->GetShapeOffset());
	//rb->SetRotation(finalRotation);
}

// Rigidbody is kinematic, so we jump manually as well, updating the physics counterpart for collision.
void cControllerSystem::_3DP_PhyKinematicJump(const glm::vec3& stickDirection, double deltaTime)
{
	// Get used params for ease of use
	//sEntity* entity = this->curControllerEntity.entity;
	//nPhysics::iRigidbody* rb = this->curControllerEntity.rigidbodyC;
	//s3DPersonControllerComponent* controller = this->curControllerEntity.tdController;
	//s3DPersonControllerComponent::sJumpState* jumpState = controller->GetJumpState();

	//if (this->_pIOManager->IsKeyDown(GLFW_KEY_SPACE) && controller->GetIsGrouded())
	//{
	//	// Set jump state to UP (launching into the jump)
	//	jumpState->_state = s3DPersonControllerComponent::sJumpState::UP;

	//	// Calculate and set jump velocity based on up time
	//	jumpState->_jumpVelocity = jumpState->_maxUpTime;
	//	jumpState->_directionVelocity = stickDirection * controller->GetMaxMoveSpeed() * 0.5f;
	//}

	//this->_CheckForGrounded(entity->transform->position, glm::vec3(0.0f, -1.0f, 0.0f));

	//// Check for grounded and if so, set state to LAND
	//if (jumpState->_state == s3DPersonControllerComponent::sJumpState::FALL)
	//{
	//	// Reset velocity and set current land time
	//	jumpState->_jumpVelocity = 0.0f;
	//	jumpState->_directionVelocity = glm::vec3(0.0f);
	//	jumpState->_currentTime = jumpState->_maxLandTime;
	//	jumpState->_state = s3DPersonControllerComponent::sJumpState::LAND;
	//}

	//switch (jumpState->_state)
	//{
	//case s3DPersonControllerComponent::sJumpState::GROUNDED: // Set initial jump if state is in the beggining, grounded
	//break;
	//case s3DPersonControllerComponent::sJumpState::UP: // Update jump velocity decreasing a little every frame. If up time is done, set state to fall.

	//	// Slow velocity down
	//	jumpState->_jumpVelocity -= (float)deltaTime;

	//	// Check if jump velocity reached 0, and if so, set state to FALL
	//	if (jumpState->_jumpVelocity <= 0.0f)
	//	{
	//		jumpState->_jumpVelocity = 0.0f;
	//		jumpState->_state = s3DPersonControllerComponent::sJumpState::FALL;
	//	}

	//break;
	//case s3DPersonControllerComponent::sJumpState::FALL: // Update jump velocity increasing a little every frame

	//	// Speed velocity up
	//	jumpState->_jumpVelocity -= (float)deltaTime;

	//	// Check if jump velocity reached max and lock it
	//	if (jumpState->_jumpVelocity <= -jumpState->_maxFallVelocity)
	//	{
	//		jumpState->_jumpVelocity = -jumpState->_maxFallVelocity;
	//	}

	//break;
	//case s3DPersonControllerComponent::sJumpState::LAND: // Update jump land time and if done, set state to GROUNDED

	//	// Lower land time a little bit every frame
	//	jumpState->_currentTime -= (float)deltaTime;

	//	// Check if land time is zero and if so reset everything
	//	if (jumpState->_currentTime <= 0.0f)
	//	{
	//		jumpState->_currentTime = 0.0f;
	//		jumpState->_state = s3DPersonControllerComponent::sJumpState::GROUNDED;
	//	}

	//break;
	//}

	//// Calculate added momentum based on the jump
	//glm::vec3 addedMomentum = glm::vec3(0.0f);
	//addedMomentum.y += jumpState->_jumpVelocity;
	//addedMomentum += jumpState->_directionVelocity;

	//if (addedMomentum == glm::vec3(0.0f))
	//{
	//	return;
	//}

	//glm::vec3 newPos = entity->transform->position + addedMomentum;

	//// Move entity
	//entity->transform->position = newPos;

	//// Update physics counterpart
	//rb->SetPosition(newPos + rb->GetShapeOffset());
}

void cControllerSystem::MoveForward(float moveAmount)
{
	this->_UpdateAcceleration(moveAmount);
}

void cControllerSystem::MoveBackward(float moveAmount)
{
	this->_UpdateAcceleration(moveAmount);
}

void cControllerSystem::MoveRight(float moveAmount)
{
	this->_UpdateAngularAcceleration(moveAmount);
}

void cControllerSystem::MoveLeft(float moveAmount)
{
	this->_UpdateAngularAcceleration(moveAmount);
}

void cControllerSystem::RotateUp(float moveAmount)
{
	this->_UpdateAngularAcceleration(moveAmount);
	//float newAccel = this->_currentRigidbodyC->angularAcceleration.x - deltaAngularAccel;
	//this->_currentRigidbodyC->angularAcceleration.x = ((newAccel <= minAngularAccel) ? minAngularAccel : this->_currentRigidbodyC->angularAcceleration.x - deltaAngularAccel);
}

void cControllerSystem::RotateDown(float moveAmount)
{
	this->_UpdateAngularAcceleration(moveAmount);
	//float newAccel = this->_currentRigidbodyC->angularAcceleration.x + deltaAngularAccel;
	//this->_currentRigidbodyC->angularAcceleration.x = ((newAccel >= maxAngularAccel) ? maxAngularAccel : this->_currentRigidbodyC->angularAcceleration.x + deltaAngularAccel);
}

void cControllerSystem::RotateRight(float moveAmount)
{
	this->_UpdateAngularAcceleration(moveAmount);
	//float newAccel = this->_currentRigidbodyC->angularAcceleration.y - deltaAngularAccel;
	//this->_currentRigidbodyC->angularAcceleration.y = ((newAccel <= minAngularAccel) ? minAngularAccel : this->_currentRigidbodyC->angularAcceleration.y - deltaAngularAccel);
}

void cControllerSystem::RotateLeft(float moveAmount)
{
	this->_UpdateAngularAcceleration(moveAmount);
	//float newAccel = this->_currentRigidbodyC->angularAcceleration.y + deltaAngularAccel;
	//this->_currentRigidbodyC->angularAcceleration.y = ((newAccel >= maxAngularAccel) ? maxAngularAccel : this->_currentRigidbodyC->angularAcceleration.y + deltaAngularAccel);
}

void cControllerSystem::RotateBarrelLeft(float moveAmount)
{
	this->_UpdateAngularAcceleration(moveAmount);
	//float newAccel = this->_currentRigidbodyC->angularAcceleration.z - deltaAngularAccel;
	//this->_currentRigidbodyC->angularAcceleration.z = ((newAccel <= minAngularVelocity) ? minAngularVelocity : this->_currentRigidbodyC->angularAcceleration.z - deltaAngularAccel);
}

void cControllerSystem::RotateBarrelRight(float moveAmount)
{
	this->_UpdateAngularAcceleration(moveAmount);
	//float newAccel = this->_currentRigidbodyC->angularAcceleration.z + deltaAngularAccel;
	//this->_currentRigidbodyC->angularAcceleration.z = ((newAccel >= maxAngularVelocity) ? maxAngularVelocity : this->_currentRigidbodyC->angularAcceleration.z + deltaAngularAccel);
}

void cControllerSystem::_UpdateAcceleration(float val)
{
	this->_currentRigidbodyC->AddForceAtPosition(-this->_cameraC->GetLookAtInWorldSpace(), this->_cameraC->eyePosition, nPhysics::ForceMode::Acceleration);

	//if (this->_currentRigidbodyC->GetRigidbodyType() == "SimpleRigidbody")
	//{
	//	nPhysics::sSimpleRigidbodyComponent* rigidbodyC = static_cast<nPhysics::sSimpleRigidbodyComponent*>(this->_currentRigidbodyC);
	//	rigidbodyC->AddRelativeForce(glm::vec3(0.0f, 0.0f, val), nPhysics::ForceMode::Force);
	//	/*float newAccel = rigidbodyC->acceleration.z + val;
	//	newAccel = nPhysicsHelper::Clamp(newAccel, min, max);
	//	rigidbodyC->acceleration.z = newAccel;*/
	//}

	//if (this->_currentRigidbodyC->GetRigidbodyType() == "BulletRigidbody")
	//{
	//	nPhysics::sBulletRigidbodyComponent* rigidbodyC = static_cast<nPhysics::sBulletRigidbodyComponent*>(this->_currentRigidbodyC);
	//	sLocationComponent* locationC = static_cast<sLocationComponent*>(rigidbodyC->location);
	//	rigidbodyC->AddRelativeForce(glm::vec3(0.0f, 0.0f, val), locationC->position, nPhysics::ForceMode::Force);
	//}
}

void cControllerSystem::_UpdateAngularAcceleration(float val)
{
	//nPhysics::iRigidbody* rigidbodyC = static_cast<nPhysics::sSimpleRigidbodyComponent*>(this->_currentRigidbodyC);
	sLocationComponent* locationC = this->_currentEntity->GetComponent<sLocationComponent>();
	glm::quat orientation = nQuaternionHelper::AdjOrientationEulerAngles(locationC->GetWorldRotation(), glm::vec3(0.0f, val, 0.0f), true);
	locationC->SetWorldRotation(locationC->GetWorldRotation() * orientation);

	//if (this->_currentRigidbodyC->GetRigidbodyType() == "SimpleRigidbody")
	//{
	//	nPhysics::sSimpleRigidbodyComponent* rigidbodyC = static_cast<nPhysics::sSimpleRigidbodyComponent*>(this->_currentRigidbodyC);
	//	rigidbodyC->AddRelativeTorque(glm::vec3(0.0f, val, 0.0f), nPhysics::ForceMode::Force);
	//	/*nPhysics::sSimpleRigidbodyComponent* rigidbodyC = static_cast<nPhysics::sSimpleRigidbodyComponent*>(this->_currentRigidbodyC);
	//	float newAccel = rigidbodyC->angularAcceleration.z + val;
	//	newAccel = nPhysicsHelper::Clamp(newAccel, min, max);
	//	rigidbodyC->angularAcceleration.z = newAccel;*/
	//}
}

void cControllerSystem::FirePrimary(sEntity* entity)
{
	printf("Fire!\n");

	// Get next bullet from the bullet pool
	sEntity* bulletEntity = cPoolManager::GetInstance()->GetEntityByName("PlayerBullet");

	// Set bullet at the muzzle position and orientate it
	nPhysics::iRigidbody* rigidbodyC = bulletEntity->GetComponent<nPhysics::iRigidbody>();
	sLocationComponent* bulletLocationC = rigidbodyC->GetTransformComponent();
	sLocationComponent* locationC = entity->GetComponent<sLocationComponent>();
	//sFlightControllerComponent* flyControllerC = static_cast<sFlightControllerComponent*>(entity->GetComponent(eComponentTypes::FLIGHTCONTROLLER_COMPONENT));
	bulletLocationC->SetWorldPosition(locationC->GetWorldPosition() + locationC->GetForwardVector() * 2.0f);
	bulletLocationC->SetWorldRotation(locationC->GetWorldRotation());

	// Activate
	bulletEntity->SetIsActive(true);

	// Add force to the bullet
	rigidbodyC->SetIsKinematic(false);
	rigidbodyC->AddForce(locationC->GetForwardVector() * 1000.0f, nPhysics::ForceMode::Force);

	// Reset timer for next firing
	//flyControllerC->currentCoolDown = flyControllerC->maxCoolDown;
	//flyControllerC->coolDown = true;
}

void cControllerSystem::CleanUp()
{
	for (unsigned int i = 0; i < this->_vecControllers.size(); i++)
	{
		delete this->_vecControllers[i];
		this->_vecControllers[i] = nullptr;
	}
}

// For safekeeping
//void cControllerSystem::_Update3DPersonController(sEntity* entity, iComponent* controller, double deltaTime)
//{
//	s3DPersonControllerComponent* controllerC = static_cast<s3DPersonControllerComponent*>(controller);
//	if (!controllerC->GetIsControlling())
//	{
//		sMeshComponent* meshC = entity->GetTComponent<sMeshComponent*>(MESH_COMPONENT);
//		meshC->animator->SetCurrentAnimation(controllerC->idleName);
//		return;
//	}
//
//	if (!controllerC->bUsePhysics)
//	{
//		if (glfwGetKey(window, GLFW_KEY_W))
//		{
//			entity->transform->position.z += 10.0f * (float)deltaTime;
//		}
//		else if (glfwGetKey(window, GLFW_KEY_S))
//		{
//			entity->transform->position.z -= 10.0f * (float)deltaTime;
//		}
//
//		if (glfwGetKey(window, GLFW_KEY_A))
//		{
//			entity->transform->position.x += 10.0f * (float)deltaTime;
//		}
//		else if (glfwGetKey(window, GLFW_KEY_D))
//		{
//			entity->transform->position.x -= 10.0f * (float)deltaTime;
//		}
//
//		if (glfwGetKey(window, GLFW_KEY_Q))
//		{
//			entity->transform->position.x += 10.0f * (float)deltaTime;
//		}
//		else if (glfwGetKey(window, GLFW_KEY_E))
//		{
//			entity->transform->position.x -= 10.0f * (float)deltaTime;
//		}
//	}
//	else
//	{
//		nPhysics::iRigidbody* rigidbodyC = entity->GetTComponent<nPhysics::iRigidbody*>(eComponentTypes::RIGIDBODY_COMPONENT);
//		this->_currentRigidbodyC = rigidbodyC;
//
//		// Hold shift to run
//		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
//		{
//			controllerC->SetIsRunning(true);
//			controllerC->SetMaxSpeed(20.0f);
//		}
//		else
//		{
//			controllerC->SetIsRunning(false);
//			controllerC->SetMaxSpeed(10.0f);
//		}
//
//		float horizontal = 0.0f;
//		float vertical = 0.0f;
//		if (glfwGetKey(window, GLFW_KEY_W))
//		{
//			vertical = 1.0f;
//		}
//		else if (glfwGetKey(window, GLFW_KEY_S))
//		{
//			vertical = -1.0f;
//		}
//		if (glfwGetKey(window, GLFW_KEY_A))
//		{
//			horizontal = 1.0f;
//		}
//		else if (glfwGetKey(window, GLFW_KEY_D))
//		{
//			horizontal = -1.0f;
//		}
//
//		if (glfwGetKey(window, GLFW_KEY_E))
//		{
//			horizontal = -1.0f;
//			controllerC->SetIsStrafing(true);
//		}
//		else if (glfwGetKey(window, GLFW_KEY_Q))
//		{
//			horizontal = 1.0f;
//			controllerC->SetIsStrafing(true);
//		}
//		else
//		{
//			controllerC->SetIsStrafing(false);
//		}
//
//		sMeshComponent* meshC = entity->GetTComponent<sMeshComponent*>(MESH_COMPONENT);
//
//		if (glfwGetKey(window, GLFW_KEY_SPACE) && !controllerC->GetIsStrafing())
//		{
//			std::string animName = controllerC->Jump();
//			meshC->animator->SetCurrentAnimation(animName);
//
//			if (controllerC->GetIsRunning())
//			{
//				glm::vec3 newPos = rigidbodyC->GetPosition() + rigidbodyC->GetTransformComponent()->direction * controllerC->GetMaxSpeed() * 20.0f * (float)deltaTime;
//				rigidbodyC->GetTransformComponent()->position = newPos - rigidbodyC->GetShapeOffset();
//				rigidbodyC->SetPosition(newPos);
//			}
//			else
//			{
//				glm::vec3 newPos = rigidbodyC->GetPosition() + rigidbodyC->GetTransformComponent()->direction * controllerC->GetMaxSpeed() * 10.0f * (float)deltaTime;
//				rigidbodyC->GetTransformComponent()->position = newPos - rigidbodyC->GetShapeOffset();
//				rigidbodyC->SetPosition(newPos);
//			}
//		}
//
//		std::string animName = controllerC->CheckForGrounded(deltaTime);
//		if (animName != "")
//		{
//			meshC->animator->SetCurrentAnimation(animName);
//		}
//
//		if (!controllerC->GetIsGrouded())
//		{
//			return;
//		}
//
//		if (glfwGetKey(window, GLFW_KEY_Z))
//		{
//			std::string animName = controllerC->Attack(0);
//			if (animName != "")
//			{
//				meshC->animator->SetCurrentAnimation(animName);
//			}
//		}
//
//		controllerC->CheckAttack(deltaTime);
//
//		if (controllerC->GetIsAttacking())
//		{
//			return;
//		}
//
//		glm::vec3 moveDirection = glm::vec3(horizontal, 0.0f, vertical);
//
//		if (moveDirection != glm::vec3(0.0f))
//		{
//			moveDirection = glm::normalize(moveDirection);
//
//			// Move kinematic rigidbody
//			//glm::vec3 newPos = rigidbodyC->GetPosition() + rigidbodyC->GetTransformComponent()->direction * controllerC->GetMaxSpeed() * (float)deltaTime;
//			//rigidbodyC->GetTransformComponent()->position = newPos - rigidbodyC->GetShapeOffset();
//			//rigidbodyC->SetPosition(newPos);
//			rigidbodyC->SetVelocity(rigidbodyC->GetTransformComponent()->direction * controllerC->GetMaxSpeed() * 20.0f * (float)deltaTime);
//
//			// Check strafing
//			if (controllerC->GetIsStrafing())
//			{
//				controllerC->SetIsRunning(true);
//				controllerC->SetMaxSpeed(20.0f);
//
//				if (glfwGetKey(window, GLFW_KEY_SPACE))
//				{
//					//newPos += moveDirection * controllerC->GetMaxSpeed() * 5.0f * (float)deltaTime;
//					//rigidbodyC->GetTransformComponent()->position = newPos - rigidbodyC->GetShapeOffset();
//					//rigidbodyC->SetPosition(newPos);
//					meshC->animator->SetCurrentAnimation("Roll");
//					return;
//				}
//
//				if (moveDirection.x <= 0.0f)
//				{
//					meshC->animator->SetCurrentAnimation("StrafeRight");
//					return;
//				}
//				else
//				{
//					meshC->animator->SetCurrentAnimation("StrafeLeft");
//					return;
//				}
//			}
//
//			rigidbodyC->GetTransformComponent()->qOrientation = nQuaternionHelper::AdjOrientationEulerAngles(rigidbodyC->GetTransformComponent()->qOrientation, glm::vec3(0.0f, 2.0f * moveDirection.x, 0.0f), true);
//
//			//glm::quat targetRotation = glm::quatLookAt(-moveDirection, glm::vec3(0.0f, 1.0f, 0.0f));
//			//float angle = glm::acos(glm::min(1.0f, glm::dot(entity->transform->direction, moveDirection)));
//			//float angleDeg = glm::degrees(angle);
//
//			//if (angleDeg > 1.0f)
//			//{
//			//	float timeElapsed = 1.0f / angle;
//
//			//	// Call Slerp
//			//	glm::quat finalRotation = glm::slerp(rigidbodyC->GetRotation(), targetRotation, 0.1f);
//			//	rigidbodyC->GetTransformComponent()->qOrientation = finalRotation;
//			//	rigidbodyC->SetRotation(finalRotation);
//			//}
//
//			if (!controllerC->GetIsRunning())
//			{
//				meshC->animator->SetCurrentAnimation(controllerC->walkName);
//			}
//			else
//			{
//				meshC->animator->SetCurrentAnimation(controllerC->runName);
//			}
//		}
//		else
//		{
//			meshC->animator->SetCurrentAnimation(controllerC->idleName);
//			rigidbodyC->SetVelocity(glm::vec3(0.0f, rigidbodyC->GetVelocity().y, 0.0f));
//		}
//	}
//}