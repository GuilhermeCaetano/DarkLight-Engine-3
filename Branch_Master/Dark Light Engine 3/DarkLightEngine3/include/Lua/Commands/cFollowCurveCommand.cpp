// cFollowCurveCommand.cpp

#include "cFollowCurveCommand.h"
#include "rapidjson\document.h"
#include "cDrawCall.h"
#include "cSeneUtils.h"
#include "glmHeaders.h"
#include "Lerp.h"
#include <glm\gtx\spline.hpp>
#include <iostream>

cFollowCurveCommand::cFollowCurveCommand(unsigned long long uniqueID)
{
	this->_ID = uniqueID;
	this->_type = "FollowCurve";

	this->_followObject = nullptr;
	this->_currentPosition = glm::vec3(0.0f);
	this->_currentDirection = glm::vec3(0.0f);
	this->_currentOrientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	this->_currentBezierIndex = 0;
	this->_currentMoveTime = 0.0f;
	this->_currentLerpVelocity = 0.0f;

	this->debugBezierPoint = glm::vec3(0.0f);

	this->_speed = 0.0f;
	this->_setup = false;
	this->_bBezierPathFinished = false;
	this->_bFinishedFollowingBezier = false;
	this->_bIsFinished = false;
	this->_bIsInfinite = false;
	this->_lifeTime = 0;

	return;
}

cFollowCurveCommand::~cFollowCurveCommand()
{
	return;
}

// Takes a json string that contains name:value pairs
void cFollowCurveCommand::Setup(rapidjson::StringBuffer& buffer)
{
	rapidjson::Document document;
	document.Parse(buffer.GetString());

	// The points will be passed as a series of numbers, each 3 representing a point, so...
	cSceneUtils* tempUtils = cSceneUtils::GetInstance();
	if (document.FindMember("FollowObject")->value.GetInt() != -1)
	{
		this->_followObject = tempUtils->FindObjectByIndex(document.FindMember("FollowObject")->value.GetInt());
	}
	else
	{
		this->_currentPosition = tempUtils->GetCurrentCamera()->eyePosition;
	}
	rapidjson::Value::Array v = document.FindMember("ControlPoints")->value.GetArray();
	for (int i = 0; i != v.Size(); i += 3)
	{
		// Go through the array and pick 3 values at a time, updating i appropriately
		glm::vec3 controlPoint = glm::vec3(v[i].GetDouble(), v[i + 1].GetDouble(), v[i + 2].GetDouble());
		this->_controlPoints.push_back(controlPoint);
	}
	this->_moveTime = document.FindMember("MoveTime")->value.GetDouble();
	this->_speed = document.FindMember("Speed")->value.GetDouble();
	this->_lifeTime = document.FindMember("LifeTime")->value.GetInt();
	this->_bIsInfinite = (this->_lifeTime == -1);

	//CalculateBezierPoints();
	
	//this->_movingToBezierPoint = 1;
	//OrientateImmediately();
	this->_currentOrientation = this->_followObject->GetMeshQuatOrientation();
	//this->_currentMaxDistance = abs(glm::distance(this->_controlPoints[this->_currentBezierIndex + 1], this->_followObject->position));
}

void cFollowCurveCommand::DrawBezier()
{
	for (unsigned int i = 0; i != this->_pointsAlongBezier.size(); i++)
	{
		cMeshObject* debugSphere = cSceneUtils::GetInstance()->FindObjectByFriendlyName("DebugSphere");
		debugSphere->position = this->_pointsAlongBezier[i];
		debugSphere->bIsVisible = true;
		DrawCall(cSceneUtils::GetInstance()->getShaderIDFromFriendlyName("myShader"), debugSphere);
		debugSphere->bIsVisible = false;
		debugSphere->nonUniformScale = glm::vec3(1.0f);
		debugSphere->position = glm::vec3(0.0f);

		if (i != this->_pointsAlongBezier.size() - 1)
		{
			glm::vec3 currentPoint = this->_pointsAlongBezier[i];
			glm::vec3 nextPoint = this->_pointsAlongBezier[i + 1];
			glLineWidth(2.0f);
			glColor3f(1.0f, 0.0f, 1.0f);
			glBegin(GL_LINES);
			glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
			glVertex3f(nextPoint.x, nextPoint.y, nextPoint.z);
			glEnd();
		}
	}
}

glm::vec3 cFollowCurveCommand::GetBezierPoint(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 point4, float t)
{

	/*return 0.5f * ((2.0f * point2)
		+ t * ((-point1 + point3)
			+ t * ((2.0f * point1 - 5.0f * point2 + 4.0f * point3 - point4)
				+ t * (-point1 + 3.0f * point2 - 3.0f * point3 + point4)))); */

	float u = 1.0f - t;
	float t2 = t * t;
	float u2 = u * u;
	float u3 = u2 * u;
	float t3 = t2 * t;

	glm::vec3 result = (u3) * point1 +
		(3.0f * u2 * t) * point2 +
		(3.0f * u * t2) * point3 +
		(t3) * point4;

	return result;
}

// https://www.opengl.org/discussion_boards/showthread.php/131788-How-to-draw-a-bezier-curve-through-its-control-points
void cFollowCurveCommand::CalculateBezierPoints()
{
	// Draw the bezier curve for testing
	int numPoints = 60;

	// Interval where the bezier points will be between the control points. So, if we have 10 control points and 20 bezier points,
	// each bezier point would show up every 1/2 between each control point, so 20 points between 10 points.
	float bezierInterval = (this->_controlPoints.size() - 1) / (numPoints - 1);
	for (unsigned int i = 0; i != numPoints; i++)
	{
		float currentInterval = i * bezierInterval; // This is the current bezier interval we are in
		this->_pointsAlongBezier.push_back(CatMullRom(currentInterval)); // Create and push back a bezier point, calling catmullrom
	}
}

// The CatMullRom is a function that uses 4 control points to define a bezier curve using a plynomial expression
glm::vec3 cFollowCurveCommand::CatMullRom(float currentBezierInterval)
{
	// First, check for the cases of a point being lower then the minimum or
	// higher than the maximum control points. So 0 < cbi < max.
	if (currentBezierInterval <= 0)
	{
		return this->_controlPoints[0];
	}
	else if (currentBezierInterval >= this->_controlPoints.size())
	{
		return this->_controlPoints[this->_controlPoints.size() - 1];
	}

	// We need an equation that will take the currentbezierinterval and will
	// get us the 4 control points around it to smooth the curve.
	int u0 = int(currentBezierInterval);

	// u relative to the start of the currentbezierinterval
	float u = currentBezierInterval - u0;

	// Now we can get the 4 control points for the bezier
	glm::vec3 tempControlPoints[4];
	for (int i = 0; i != 4; i++)
	{
		int index = u0 - 1 + i;
		index = index < 0 ? 0 : index;
		index = index >= this->_controlPoints.size() ? this->_controlPoints.size() - 1 : index;

		tempControlPoints[i] = this->_controlPoints[index];
	}

	// Evaluate the polynomial at u
	float X = 0.5f * ((2.0f * tempControlPoints[1].x)
		+ u * ((-tempControlPoints[0].x + tempControlPoints[2].x) 
		+ u * ((2.0f * tempControlPoints[0].x - 5.0f * tempControlPoints[1].x + 4.0f * tempControlPoints[2].x - tempControlPoints[3].x)
		+ u * (-tempControlPoints[0].x + 3.0f * tempControlPoints[1].x - 3.0f * tempControlPoints[2].x + tempControlPoints[3].x))));

	float Y = 0.5f * ((2.0f * tempControlPoints[1].y)
		+ u * ((-tempControlPoints[0].y + tempControlPoints[2].y)
		+ u * ((2.0f * tempControlPoints[0].y - 5.0f * tempControlPoints[1].y + 4.0f * tempControlPoints[2].y - tempControlPoints[3].y)
		+ u * (-tempControlPoints[0].y + 3.0f * tempControlPoints[1].y - 3.0f * tempControlPoints[2].y + tempControlPoints[3].y))));

	float Z = 0.5f * ((2.0f * tempControlPoints[1].z)
		+ u * ((-tempControlPoints[0].z + tempControlPoints[2].z)
		+ u * ((2.0f * tempControlPoints[0].z - 5.0f * tempControlPoints[1].z + 4.0f * tempControlPoints[2].z - tempControlPoints[3].z)
		+ u * (-tempControlPoints[0].z + 3.0f * tempControlPoints[1].z - 3.0f * tempControlPoints[2].z + tempControlPoints[3].z))));

	return glm::vec3(X, Y, Z);
	/*return 0.5f * ((2.0f * tempControlPoints[1])
		+ u * ((-tempControlPoints[0] + tempControlPoints[2])
		+ u * ((2.0f * tempControlPoints[0] - 5.0f * tempControlPoints[1] + 4.0f * tempControlPoints[2] - tempControlPoints[3])
		+ u * (-tempControlPoints[0] + 3.0f * tempControlPoints[1] - 3.0f * tempControlPoints[2] + tempControlPoints[3]))));*/
}

// Called every frame to check functionality
void cFollowCurveCommand::Update(double deltaTime)
{
	//DrawBezier();

	if (!this->_setup)
	{
		this->_setup = true;
		this->_followObject->position = this->_controlPoints[0];
	}

	if (IsFinished())
	{
		return;
	}

	// Join movement and rotational behaviours
	UpdateMovement(deltaTime);
	//UpdateOrientation(deltaTime);
}

void cFollowCurveCommand::UpdateMovement(double deltaTime)
{
	// Update object movement along the bezier curve
	if (IsFinished())
	{
		return;
	}

	//int bIndex = this->_currentBezierIndex;
	//// Try to approximate the curve arc-length
	//float chord = (this->_controlPoints[bIndex + 3] - this->_controlPoints[bIndex]).length();
	//float contNet = abs(glm::distance(this->_controlPoints[bIndex], this->_controlPoints[bIndex + 1]))
	//				+ abs(glm::distance(this->_controlPoints[bIndex + 2], this->_controlPoints[bIndex + 1]))
	//				+ abs(glm::distance(this->_controlPoints[bIndex + 3], this->_controlPoints[bIndex + 2]));
	//float arcLength = (chord + contNet) / 2.0f;

	//this->_currentMoveTime += 1.0f * float(deltaTime);
	//float t = this->_currentMoveTime / arcLength;
	//

	float moveTime = this->_moveTime;
	float t = this->_currentMoveTime;
	int bIndex = this->_currentBezierIndex;

	if (!this->_bBezierPathFinished)
	{
		t += (1.0f / moveTime) * float(deltaTime); // between 0 and 1
		this->_currentMoveTime = t;
	}
	
	glm::vec3 bezierPoint = this->GetBezierPoint(this->_controlPoints[bIndex], this->_controlPoints[bIndex + 1], this->_controlPoints[bIndex + 2], this->_controlPoints[bIndex + 3], t);
	UpdateOrientation(deltaTime, bezierPoint);
	cMeshObject* debugSphere = cSceneUtils::GetInstance()->FindObjectByFriendlyName("DebugSphere");
	debugSphere->bIsVisible = true;
	for (int i = bIndex; i != bIndex + 4; i++)
	{
		debugSphere->position = this->_controlPoints[i];
		debugSphere->materialDiffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		DrawCall(cSceneUtils::GetInstance()->getShaderIDFromFriendlyName("myShader"), debugSphere);
	}

	debugSphere->position = bezierPoint;
	debugSphere->materialDiffuse = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	DrawCall(cSceneUtils::GetInstance()->getShaderIDFromFriendlyName("myShader"), debugSphere);
	debugSphere->bIsVisible = false;
	debugSphere->position = glm::vec3(0.0f);

	float distance = abs(glm::distance(bezierPoint, this->_followObject->position));

	//this->_followObject->position = bezierPoint;  // Using it this way will lerp during the whole curve, depending on the distance between the points
	// Like this, we move the object towards the curve point, being careful with cases where it's too close (resulting in nan), in this case, keep moving
	// on the same direction we saved before, until it's safe to move again
	if (distance > 0.1f)
	{
		if (this->_currentBezierIndex == 0 && !this->_bBezierPathFinished)
		{
			// Moving out the first time, lerp
			
			float v = this->_currentLerpVelocity;

			v += (float)deltaTime;
			v = 1.0f / (2 - v);
			//v = (v + (15.0f/moveTime) * 0.4f) * float(deltaTime) >= 1.0f ? 1.0f : (v + (15.0f/moveTime) * 0.4f) * float(deltaTime);

			v = LerpTo(0.0f, 1.0f, v);
			glm::vec3 direction = bezierPoint - this->_followObject->position;
			direction = glm::normalize(direction);
			this->_currentDirection = direction;

			glm::vec3 dirVelocity = direction * float(deltaTime);
			this->_followObject->position += dirVelocity * this->_currentLerpVelocity * this->_speed;
			this->_currentLerpVelocity = v;
		}
		else if (this->_bBezierPathFinished)
		{
			// Last point, smooth lerp to it
			float finalDist = abs(glm::distance(bezierPoint, this->_followObject->position));
			finalDist = (finalDist > 1.0f) ? 1.0f : finalDist;
			float invD = 1 - finalDist;

			float slowDown = LerpToPrecise(1.0f, 0.0f, invD);
			glm::vec3 direction = bezierPoint - this->_followObject->position;
			direction = glm::normalize(direction);
			this->_currentDirection = direction;

			glm::vec3 dirVelocity = direction * float(deltaTime);
			this->_followObject->position += dirVelocity * this->_speed * slowDown;
		}
		else
		{
			glm::vec3 direction = bezierPoint - this->_followObject->position;
			direction = glm::normalize(direction);
			this->_currentDirection = direction;

			glm::vec3 dirVelocity = direction * float(deltaTime);
			this->_followObject->position += dirVelocity * this->_speed;
		}
	}
	else
	{
		if (this->_bBezierPathFinished)
		{
			this->_bFinishedFollowingBezier = true;
			return;
		}

		// Keep moving on the same direction, until we can follow the point again
		glm::vec3 dirVelocity = this->_currentDirection * float(deltaTime);
		this->_followObject->position += dirVelocity * this->_speed;
	}

	if (t >= 1.0f)
	{
		if (this->_currentBezierIndex == this->_controlPoints.size() - 4)
		{
			if (this->_bIsInfinite)
			{
				Reset();
				return;
			}

			this->_lifeTime--;
			if (this->_lifeTime > 0)
			{
				Reset();
				return;
			}

			this->_bBezierPathFinished = true;
		}
		else
		{
			this->_currentBezierIndex += 3;
			this->_currentMoveTime = 0.0f;
		}
	}

	//this->_currentMoveTime += (1.0f / moveTime) * float(deltaTime);
	//this->_currentMoveTime += 2.0f * float(deltaTime);

	// TEST TO SEE LERP
	//float movementThisFrame = abs(glm::distance(this->debugBezierPoint, bezierPoint));
	//std::cout << movementThisFrame << std::endl;
	//this->debugBezierPoint = bezierPoint;


	/*glm::vec3 direction = bezierPoint - this->_followObject->position;
	float distance = direction.length();
	direction = glm::normalize(direction);

	if (abs(distance) > 0.2f)
	{
		glm::vec3 dirVelocity = direction * float(deltaTime);
		this->_followObject->position += dirVelocity;
		this->_currentMoveTime += float(deltaTime);
	}
	else
	{

	}
*/

	////this->_maxSpeed = 0.01f;
	//glm::vec3 targetBezierPoint = this->_pointsAlongBezier[this->_movingToBezierPoint];
	//if (abs(glm::distance(this->_followObject->position, targetBezierPoint)) > 1.0f)
	//{
	//	// Find the line path to travel
	//	glm::vec3 direction = targetBezierPoint - this->_followObject->position;
	//	glm::vec3 normDirection = glm::normalize(direction);
	//	//glm::vec3 dirVelocity = SmoothVelocity(direction);
	//	// Smooth the velocity
	//	float distance = abs(glm::distance(this->_followObject->position, targetBezierPoint));

	//	float smoothValue = distance / this->_maxSpeed;
	//	float smooth2 = LerpTo(0.0f, 1.0f, smoothValue);


	//	/*dirVelocity *= smoothValue;
	//	dirVelocity *= (float)deltaTime;*/
	//	this->_followObject->position += normDirection * this->_maxSpeed; //smoothValue * float(deltaTime);
	//}
	//else
	//{
	//	if (this->_movingToBezierPoint == this->_pointsAlongBezier.size() - 1)
	//	{
	//		_bIsFinished = true;
	//		return;
	//	}
	//	
	//	this->_movingToBezierPoint++;
	//}
}

void cFollowCurveCommand::OrientateImmediately()
{
	// Update object rotation along the bezier curve
	glm::vec3 objectDirection = this->_followObject->GetDirection(); // This is the object's forward vector
	objectDirection = glm::normalize(objectDirection);
	objectDirection.y = 0.0f;

	glm::vec3 targetDirection = this->_controlPoints[this->_currentBezierIndex] - this->_followObject->position;
	targetDirection = glm::normalize(targetDirection);
	targetDirection.y = 0.0f;


	// Create the quaternion rotation from the two vectors
	//this->_targetRotation = RotateTowards(objectDirection, targetDirection);
	glm::quat targetRotation = glm::quatLookAt(-targetDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat ftargetRotation = glm::inverse(this->_currentOrientation) * targetRotation;

	this->_followObject->setMeshOrientationQ(targetRotation);
}

void cFollowCurveCommand::UpdateOrientation(double deltaTime, glm::vec3 bezierPoint)
{
	// Update object rotation along the bezier curve
	glm::vec3 objectDirection = this->_followObject->GetDirection(); // This is the object's forward vector
	objectDirection = glm::normalize(objectDirection);
	objectDirection.y = 0.0f;

	glm::vec3 targetDirection = bezierPoint - this->_followObject->position;
	targetDirection = glm::normalize(targetDirection);
	targetDirection.y = 0.0f;


	// Create the quaternion rotation from the two vectors
	//this->_targetRotation = RotateTowards(objectDirection, targetDirection);
	glm::quat _targetRotation = glm::quatLookAt(-targetDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat ftargetRotation = glm::inverse(this->_currentOrientation) * _targetRotation;

	// Find the angle between the vectors and check for the slerp
	float angle = glm::acos(glm::min(1.0f, glm::dot(objectDirection, targetDirection)));
	angle = glm::degrees(angle);
	float angle2 = glm::angle(_targetRotation);
	angle2 = glm::degrees(angle2);
	if (glm::abs(angle) < 1.0f)
	{

	}
	else
	{
		float timeElapsed = 1.0f / angle;
		//glm::quat dist = glm::distance(_currentRotation, _targetRotation);
		// Call Slerp
		//glm::quat finalRotation = glm::mix(_currentRotation, _targetRotation, timeElapsed);
		glm::quat finalRotation = glm::slerp(this->_currentOrientation, _targetRotation, 0.1f);
		//glm::quat finalRotation = SlerpTo(_currentRotation, _targetRotation, timeElapsed * 0.001f);
		//finalRotation *= _currentRotation;
		//this->_rotateObject->adjMeshOrientationQ(_rotateObject->GetMeshQuatOrientation(), finalRotation);

		//this->_currentRotation = finalRotation;

		//_rotateObject->GetMeshQuatOrientation() = finalRotation;
		//this->_rotateObject->setMeshOrientationQ(finalRotation);
		//this->_rotateObject->GetMeshQuatOrientation() = _targetRotation;
		//this->_rotateObject->adjMeshOrientationQ(_rotateObject->GetMeshQuatOrientation(), finalRotation);
		this->_followObject->setMeshOrientationQ(finalRotation);
		this->_currentOrientation = finalRotation;
	}
}

void cFollowCurveCommand::Reset()
{
	if (this->_bIsInfinite)
	{
		this->_bIsFinished = false;
		this->_bBezierPathFinished = false;
		this->_bFinishedFollowingBezier = false;

		this->_currentBezierIndex = 0;
		this->_currentMoveTime = 0.0f;
		this->_currentLerpVelocity = 0.0f;
		this->_followObject->position = this->_controlPoints[0];

		this->_currentOrientation = this->_followObject->GetMeshQuatOrientation();
	}
}

bool cFollowCurveCommand::IsFinished()
{
	if (this->_bBezierPathFinished && this->_bFinishedFollowingBezier)
	{
		this->_bIsFinished = true;
	}

	return this->_bIsFinished;
}

glm::vec3 cFollowCurveCommand::GetCurrentPosition()
{
	return this->_currentPosition;
}