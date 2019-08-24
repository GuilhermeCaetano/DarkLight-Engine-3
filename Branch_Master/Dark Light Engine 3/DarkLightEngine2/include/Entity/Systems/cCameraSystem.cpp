// cCameraSystem.cpp

#include <OpenGL\OpenGLHeaders.h>
#include <Math\nQuaternionHelper.h>
#include <Pools\cPoolManager.h>
#include <Entity\Systems\cCameraSystem.h>
#include <Entity\Components\sCameraComponent.h>
#include <Entity\Components\sLocationComponent.h>
#include <Math\nMathHelper.h>

cCameraSystem::cCameraSystem()
{
	this->_followIndex = 0;

	return;
}

cCameraSystem::~cCameraSystem()
{
	return;
}

void cCameraSystem::SetCameraUniformLocations(GLuint shaderProgramID)
{
	this->_cameraLocation = glGetUniformLocation(shaderProgramID, "eyeLocation");
	this->_matViewLocation = glGetUniformLocation(shaderProgramID, "matView");
}

void cCameraSystem::SetCameraFollowTarget(sLocationComponent* location)
{
	this->_vecFollowTargets.push_back(location);
}

void cCameraSystem::UpdateCameraView(iComponent* component)
{
	sCameraComponent* cameraC = static_cast<sCameraComponent*>(component);
	this->_currentCamera = cameraC;

	this->_currentCamera->UpdateAtFromOrientation();
	this->_currentCamera->UpdateForwardFromOrientation();
	this->_currentCamera->UpdateUpFromOrientation();
	this->_currentCamera->UpdateRightFromOrientation();

	if (cIOManager::GetEditorState() != cIOManager::Free)
	{
		return;
	}

	if (cameraC->target != nullptr && cameraC->bFollowTarget)
	{
		glm::mat4x4 matView = glm::mat4(1.0f); // Identity

		glm::vec3 targetLookAt = cameraC->target->transform->GetWorldPosition() + cameraC->target->transform->GetRightVector() * cameraC->lookAtOffset.x
			+ cameraC->target->transform->GetUpVector() * cameraC->lookAtOffset.y + cameraC->target->transform->GetForwardVector() * cameraC->lookAtOffset.z;

		matView = glm::lookAt(cameraC->eyePosition, targetLookAt, cameraC->GetUpVector());
		glm::quat newOrientation = glm::conjugate(glm::toQuat(matView));
		//glm::quat newOrientation = glm::toQuat(matView);


		// Assimilate Z rotation
		/*glm::vec3 eulerRot = glm::eulerAngles(cameraC->target->transform->qOrientation);
		glm::vec3 cameraEulerRot = glm::eulerAngles(cameraC->qOrientation);
		cameraEulerRot.z = eulerRot.z;
		glm::quat newCameraRot = glm::quat(cameraEulerRot);*/

		cameraC->qOrientation = newOrientation;
	}
}

void cCameraSystem::UpdateSystem(double deltaTime)
{
	iComponent* component = nullptr;
	cPoolManager* tempManager = cPoolManager::GetInstance();
	for (unsigned int i = 0; i != tempManager->vecStaticEntities.size(); i++)
	{
		component = tempManager->vecStaticEntities[i]->GetComponent<sCameraComponent>();
		if (component != nullptr)
		{
			sCameraComponent* cameraComponent = static_cast<sCameraComponent*>(component);

			UpdateCameraView(component);
			glUniform3f(this->_cameraLocation, cameraComponent->eyePosition.x, cameraComponent->eyePosition.y, cameraComponent->eyePosition.z);
		}
	}

	//TODO: Go through the entities with a camera.
	// Will do scheduling later...
	cPoolManager* pool = cPoolManager::GetInstance();
	for (unsigned int i = 0; i != pool->vecStaticEntities.size(); i++)
	{
		sCameraComponent* cameraC = pool->vecStaticEntities[i]->GetComponent<sCameraComponent>();
		if (cameraC != nullptr)
		{
			this->UpdateCameraView(cameraC);

			if (cIOManager::GetEditorState() != cIOManager::Free)
			{
				cameraC->bFollowTarget = false;
				return;
			}

			cameraC->bFollowTarget = true;
			switch (cameraC->GetCameraFollowType())
			{
			case sCameraComponent::eFollowTypes::SIMPLE:

				this->_SimpleCameraFollow(cameraC, deltaTime);
			break;
			case sCameraComponent::eFollowTypes::SMOOTH:

				this->_SmoothCameraFollow(cameraC, deltaTime);
			break;
			case sCameraComponent::eFollowTypes::ZELDA:

				this->_ZeldaCameraFollow(cameraC, deltaTime);
			break;
			}
		}
	}
}

void cCameraSystem::_SimpleCameraFollow(iComponent* iCameraC, double deltaTime)
{
	// Check if they are set to follow and have a target
	sCameraComponent* cameraC = static_cast<sCameraComponent*>(iCameraC);

	if (cameraC->bFollowTarget && cameraC->target != nullptr)
	{
		glm::vec3 targetFollowPos = cameraC->target->transform->GetWorldPosition() + cameraC->target->transform->GetUpVector() * cameraC->targetOffset.y + cameraC->target->transform->GetForwardVector() * cameraC->targetOffset.z;
		//glm::vec3 lookAt = targetFollowPos + cameraC->target->transform->direction;

		cameraC->eyePosition = targetFollowPos;

		/*glm::quat newOrientation = glm::lookAt(cameraC->eyePosition, lookAt, glm::vec3(0.0f, 1.0f, 0.0f));
		cameraC->qOrientation = newOrientation;*/
	}
}

// In order to smooth follow a target, we must have two "areas".
// A max speed area, in which we travel to the target position at max speed,
// and a slowdown area, in which we smoothly lerp towars an ideal position.
// The lerp will use the distance from the max area to the slowdown area as the 0-1 value.
void cCameraSystem::_SmoothCameraFollow(iComponent* camera, double deltaTime)
{
	// Check for early return
	sCameraComponent* cameraC = static_cast<sCameraComponent*>(camera);
	if (!cameraC->bFollowTarget || cameraC->target == nullptr)
	{
		return;
	}

	glm::vec3 lookDirection = cameraC->target->transform->GetWorldPosition() - cameraC->eyePosition;
	lookDirection = glm::normalize(lookDirection);
	lookDirection.y = 0.0f;

	// Find the ideal position and calculate the direction to it
	glm::vec3 idealPos = cameraC->target->transform->GetWorldPosition() + cameraC->targetOffset;
	//glm::vec3 idealPos = cameraC->target->transform->position;
	//idealPos.y += cameraC->targetOffset.y;
	//idealPos -= lookDirection * 200.0f;
	//glm::vec3 idealPos = cameraC->target->transform->position + cameraC->qOrientation * (glm::vec3(0.0f, 1.0f, 0.0f) * cameraC->targetOffset.y - lookDirection * 50.0f);
	glm::vec3 targetDir = idealPos - cameraC->eyePosition;
	targetDir = glm::normalize(targetDir);

	// Find the max distance position and calculate the direction to it
	glm::vec3 maxDistPos = idealPos - targetDir * cameraC->fullSpeedDistance;
	glm::vec3 maxDistDir = maxDistPos - cameraC->eyePosition;

	// Check if the camera is ont the max distance area, which is outside the slowdown area
	float cameraDistance = fabs(glm::distance(idealPos, cameraC->eyePosition));
	if (cameraDistance > cameraC->fullSpeedDistance)
	{
		// The camera is on the max distance area so...
		// Travel at max speed
		cameraC->followVelocity = targetDir * cameraC->maxFollowSpeed;
	}
	else
	{
		// The camera is inside the slowdown area so...
		// Calculate the value 0-1 the camera is inside the area
		float percentage0to1Inside = cameraDistance / cameraC->fullSpeedDistance;

		// Calculate lerp value based on the 0-1 percentage
		float lerpSpeed = nMathHelper::Smootherstep(0.0f, cameraC->maxFollowSpeed, percentage0to1Inside);

		// Set velocity
		cameraC->followVelocity = targetDir * lerpSpeed;
	}

	// The camera velocity has been decided, so move the camera
	cameraC->eyePosition += cameraC->followVelocity * (float)deltaTime;
	
	//targetDir = nMathHelper::NormalizeVector(targetDir);
	//targetDir.y = 0.0f;

	////targetPosition = player.transform.position + cameraRotate.rotation * (player.transform.up * distanceUp - lookDirection * distanceAway);
	//glm::vec3 targetFollowPos = cameraC->target->transform->position + cameraC->qOrientation * (glm::vec3(0.0f, 1.0f, 0.0f) * cameraC->targetOffset.y - targetDir * cameraC->targetOffset.z);

	//// Do follow routine
	//// Ideal location is 'followTargetLocation'
	//glm::vec3 distanceToTarget = cameraC->eyePosition - targetFollowPos;

	//// If we are GT max distance on any axis, then go full speed
	//if (fabs(distanceToTarget.x) > cameraC->fullSpeedDistance)
	//{	// Go at maximum speed
	//	cameraC->followVelocity.x = cameraC->maxFollowSpeed * -glm::sign(distanceToTarget.x);
	//}
	//else
	//{	// Slow down when getting close
	//	cameraC->followVelocity.x = nMathHelper::Smoothstep(0.0f, cameraC->fullSpeedDistance, fabs(distanceToTarget.x))
	//		* cameraC->maxFollowSpeed
	//		* -glm::sign(distanceToTarget.x);
	//}

	//if (fabs(distanceToTarget.y) > cameraC->fullSpeedDistance)
	//{	// Go at maximum speed
	//	cameraC->followVelocity.y = cameraC->maxFollowSpeed * -glm::sign(distanceToTarget.y);
	//}
	//else
	//{	// Slow down when getting close
	//	cameraC->followVelocity.y = nMathHelper::Smoothstep(0.0f, cameraC->fullSpeedDistance, fabs(distanceToTarget.y))
	//		* cameraC->maxFollowSpeed
	//		* -glm::sign(distanceToTarget.y);
	//}


	//if (fabs(distanceToTarget.z) > cameraC->fullSpeedDistance)
	//{	// Go at maximum speed
	//	cameraC->followVelocity.z = cameraC->maxFollowSpeed * -glm::sign(distanceToTarget.z);
	//}
	//else
	//{	// Slow down when getting close
	//	cameraC->followVelocity.z = nMathHelper::Smoothstep(0.0f, cameraC->fullSpeedDistance, fabs(distanceToTarget.z))
	//		* cameraC->maxFollowSpeed
	//		* -glm::sign(distanceToTarget.z);
	//}


	//cameraC->eyePosition.x += cameraC->followVelocity.x * static_cast<float>(deltaTime);
	//cameraC->eyePosition.y += cameraC->followVelocity.y * static_cast<float>(deltaTime);
	//cameraC->eyePosition.z += cameraC->followVelocity.z * static_cast<float>(deltaTime);
}

void cCameraSystem::_ZeldaCameraFollow(iComponent* iCameraC, double deltaTime)
{
	sCameraComponent* cameraC = static_cast<sCameraComponent*>(iCameraC);
	if (!cameraC->bFollowTarget || cameraC->target == nullptr)
	{
		return;
	}

	glm::vec3 lookDirection = cameraC->target->transform->GetWorldPosition() - cameraC->eyePosition;
	lookDirection = glm::normalize(lookDirection);
	lookDirection.y = 0.0f;

	// Find the ideal position and calculate the direction to it
	//glm::vec3 idealPos = cameraC->target->transform->position + cameraC->targetOffset;
	glm::vec3 idealPos = cameraC->target->transform->GetWorldPosition();
	idealPos.y += cameraC->targetOffset.y;
	idealPos -= lookDirection * 120.0f;
	//glm::vec3 idealPos = cameraC->target->transform->position + cameraC->qOrientation * (glm::vec3(0.0f, 1.0f, 0.0f) * cameraC->targetOffset.y - lookDirection * 50.0f);
	glm::vec3 targetDir = idealPos - cameraC->eyePosition;
	targetDir = glm::normalize(targetDir);

	// Find the max distance position and calculate the direction to it
	glm::vec3 maxDistPos = idealPos - targetDir * cameraC->fullSpeedDistance;
	glm::vec3 maxDistDir = maxDistPos - cameraC->eyePosition;

	// Check if the camera is ont the max distance area, which is outside the slowdown area
	float cameraDistance = fabs(glm::distance(idealPos, cameraC->eyePosition));
	if (cameraDistance > cameraC->fullSpeedDistance)
	{
		// The camera is on the max distance area so...
		// Travel at max speed
		cameraC->followVelocity = targetDir * cameraC->maxFollowSpeed;
	}
	else
	{
		// The camera is inside the slowdown area so...
		// Calculate the value 0-1 the camera is inside the area
		float percentage0to1Inside = cameraDistance / cameraC->fullSpeedDistance;

		// Calculate lerp value based on the 0-1 percentage
		float lerpSpeed = nMathHelper::Smootherstep(0.0f, cameraC->maxFollowSpeed, percentage0to1Inside);

		// Set velocity
		cameraC->followVelocity = targetDir * lerpSpeed;
	}

	// The camera velocity has been decided, so move the camera
	cameraC->eyePosition += cameraC->followVelocity * (float)deltaTime;
}

bool cCameraSystem::ScheduleEntity(sEntity * entity, iComponent * component)
{
	return false;
}

bool cCameraSystem::UnscheduleEntity(sEntity * entity, iComponent * component)
{
	return false;
}

void cCameraSystem::ProcessKeys(GLFWwindow* window, cIOManager* pIOManager)
{
	if (cIOManager::GetEditorState() == cIOManager::Free)
	{
		return;
	}

	cPoolManager* tempPool = cPoolManager::GetInstance();
	for (unsigned int i = 0; i != tempPool->vecStaticEntities.size(); i++)
	{
		iComponent* component = tempPool->vecStaticEntities[i]->GetComponent<sCameraComponent>();
		if (component != nullptr)
		{
			sCameraComponent* cameraComponent = static_cast<sCameraComponent*>(component);
			this->_currentCamera = cameraComponent;
			if (tempPool->vecStaticEntities[i]->GetIsActive() && cameraComponent->bCanControl)
			{
				this->_ProcessKeyboard(window, pIOManager);
				this->_ProcessMouse(window, pIOManager);
			}
		}
	}
}

void cCameraSystem::_FollowNextTarget()
{
	this->_followIndex++;
	if (this->_followIndex >= this->_vecFollowTargets.size())
	{
		this->_followIndex = 0;
	}
}

void cCameraSystem::_ProcessKeyboard(GLFWwindow* window, cIOManager* pIOManager)
{
	if (cIOManager::GetEditorState() == cIOManager::Free)
	{
		return;
	}

	if (!pIOManager->IsMouseLeftButtonPressed(window))
	{
		return;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // "forward"
	{
		this->_MoveForward_Z(1.0f * this->_currentCamera->movementSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)	// "backward"
	{
		this->_MoveForward_Z(-1.0f * this->_currentCamera->movementSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)	// "left"
	{
		this->_MoveLeftRight_X(-1.0f * this->_currentCamera->movementSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)	// "right"
	{
		this->_MoveLeftRight_X(1.0f * this->_currentCamera->movementSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)	// "up"
	{
		this->_MoveUpDown_Y(1.0f * this->_currentCamera->movementSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)	// "down"
	{
		this->_MoveUpDown_Y(-1.0f * this->_currentCamera->movementSpeed);
	}
}

void cCameraSystem::_ProcessMouse(GLFWwindow* window, cIOManager* pIOManager)
{
	if (cIOManager::GetEditorState() == cIOManager::Free)
	{
		return;
	}

	// Mouse left pressed while inside screen
	if (pIOManager->IsMouseLeftButtonPressed(window))
	{
		// Mouse button is down so turn the camera
		this->_Yaw_LeftRight(-pIOManager->GetDeltaMouseX() * pIOManager->mouseSensitivity);

		this->_Pitch_UpDown(pIOManager->GetDeltaMouseY() * pIOManager->mouseSensitivity);
	}

	// Adjust the mouse speed
	if (pIOManager->IsMouseInsideWindow())
	{
		// Adjust the movement speed based on the wheel position
		this->_currentCamera->movementSpeed = pIOManager->GetMouseWheel() * pIOManager->mouseWheelSensitivity;
	}
}

void cCameraSystem::_MoveForward_Z(float amount)
{
	// Get direction to the look at position
	glm::vec3 direction = this->_currentCamera->GetLookAtInWorldSpace() - this->_currentCamera->eyePosition;
	direction = glm::normalize(direction);

	// Generate a "forward" adjustment value 
	glm::vec3 amountToMove = direction * amount;

	// Add movement to the position
	this->_currentCamera->eyePosition += amountToMove;
}

void cCameraSystem::_MoveUpDown_Y(float amount)
{
	// Get current up and normalize to form another direction
	glm::vec3 vecAmountToMove = glm::normalize(this->_currentCamera->GetUpVector()) * amount;

	// Add movement to the position
	this->_currentCamera->eyePosition += vecAmountToMove;
}

void cCameraSystem::_MoveLeftRight_X(float amount)
{
	// left and right is a little tricky, as we can "roll", 
	// because left and right change in world space.
	// 
	// If you perform a cross product between the "forward" and "up" vectors, you'll
	//	get a vector perpendicular to both of these, so left and right.
	// Example: Assuming "forward" is the Z axis, and "up" is the Y axis, then 
	//	performing the cross product of these gives you the X axis. 
	glm::vec3 vecLeft = glm::cross(this->_currentCamera->GetCameraDirection(), this->_currentCamera->GetUpVector());
	glm::vec3 vecAmountToMove = glm::normalize(vecLeft) * amount;

	// Add movement to the position
	this->_currentCamera->eyePosition += vecAmountToMove;
}


void cCameraSystem::_Pitch_UpDown(float angleDegrees)	// around X
{
	// Adjust the orientation of the camera by the number of degrees
	glm::quat newQuat = nQuaternionHelper::AdjOrientationEulerAngles(this->_currentCamera->qOrientation, glm::vec3(angleDegrees, 0.0f, 0.0f), true);
	this->_currentCamera->qOrientation = newQuat;

	// Update axis
	this->_currentCamera->UpdateAtFromOrientation();
	this->_currentCamera->UpdateUpFromOrientation();
}

void cCameraSystem::_Yaw_LeftRight(float angleDegrees)	// around y
{
	// Adjust the orientation of the camera by the number of degrees
	glm::quat newQuat = nQuaternionHelper::AdjOrientationEulerAngles(this->_currentCamera->qOrientation, glm::vec3(0.0f, angleDegrees, 0.0f), true);
	this->_currentCamera->qOrientation = newQuat;

	// Update axis
	this->_currentCamera->UpdateAtFromOrientation();
	this->_currentCamera->UpdateUpFromOrientation();
}

void cCameraSystem::_Roll_CW_CCW(float angleDegrees)	// around z
{
	// Adjust the orientation of the camera by the number of degrees
	glm::quat newQuat = nQuaternionHelper::AdjOrientationEulerAngles(this->_currentCamera->qOrientation, glm::vec3(0.0f, 0.0f, angleDegrees), true);
	this->_currentCamera->qOrientation = newQuat;

	// Update axis
	this->_currentCamera->UpdateAtFromOrientation();
	this->_currentCamera->UpdateUpFromOrientation();
}
