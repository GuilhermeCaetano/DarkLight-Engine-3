// Quaternions.cpp

#include "nQuaternionHelper.h"

// Set the quaternion by using euler angles as vec3
glm::quat nQuaternionHelper::SetOrientationEulerAngles(glm::vec3 newEulerAngle, bool bIsDegrees)
{
	if (bIsDegrees)
	{
		newEulerAngle = glm::vec3(glm::radians(newEulerAngle.x),
			glm::radians(newEulerAngle.y),
			glm::radians(newEulerAngle.z));
	}

	glm::quat newQuat = glm::quat(glm::vec3(newEulerAngle.x, newEulerAngle.y, newEulerAngle.z));
	return newQuat;
}

// Set the quaternion by using euler angles as floats
glm::quat nQuaternionHelper::SetOrientationEulerAngles(float x, float y, float z, bool bIsDegrees)
{
	glm::quat newQuat = SetOrientationEulerAngles(glm::vec3(x, y, z), bIsDegrees);
	return newQuat;
}

// Sums a rotation to the current quaternion rotation by using vec3
glm::quat nQuaternionHelper::AdjOrientationEulerAngles(glm::quat quatToAdjust, glm::vec3 adjEulerAngle, bool bIsDegrees)
{
	if (bIsDegrees)
	{
		adjEulerAngle = glm::vec3(glm::radians(adjEulerAngle.x),
			glm::radians(adjEulerAngle.y),
			glm::radians(adjEulerAngle.z));
	}

	// Step 1: make a quaternion that represents the angle we want to rotate
	glm::quat rotationAdjust(adjEulerAngle);

	// Step 2: Multiply this quaternion by the existing quaternion. This "adds" the angle we want.
	glm::quat resultQuaternion = quatToAdjust * rotationAdjust;
	return resultQuaternion;
}

// Sums a rotation to the current quaternion rotation by using floats
glm::quat nQuaternionHelper::AdjOrientationEulerAngles(glm::quat quatToAdjust, float x, float y, float z, bool bIsDegrees)
{
	glm::quat newQuat = AdjOrientationEulerAngles(quatToAdjust, glm::vec3(x, y, z), bIsDegrees);
	return newQuat;
}

// Sums a rotation to the current quaternion rotation by using another quaternion
glm::quat nQuaternionHelper::AdjOrientationQuat(glm::quat quatToAdjust, glm::quat adjOrientQ)
{
	glm::quat resultQuaternion = quatToAdjust * adjOrientQ;
	return resultQuaternion;
}

// Smooth rotation between two quaternion rotations.
glm::quat Slerp(glm::quat fromRotation, glm::quat toRotation)
{
	return glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
}

// https://stackoverflow.com/questions/1171849/finding-quaternion-representing-the-rotation-from-one-vector-to-another
// Rotate a vector to point in the same direction as another.
glm::quat nQuaternionHelper::RotateTowards(glm::vec3 vectorA, glm::vec3 vectorB)
{
	// The idea behind this is that in order to calculate the rotation wanted,
	// we first calculate a rotation with double the size and find the half way.
	// Calculating double quaternion:
	glm::quat returnQ;
	glm::vec3 quatXYZ = glm::cross(vectorA, vectorB);

	// Getting half the size
	float quatW = glm::sqrt((vectorA.length() * vectorA.length()) * (vectorB.length() * vectorB.length())) + glm::dot(vectorA, vectorB);

	returnQ = glm::quat(quatW, quatXYZ.x, quatXYZ.y, quatXYZ.z);

	return returnQ;
}

// Rotate a vector to smoothly point in the same direction as another
glm::vec3 SlerpTowards(glm::vec3 VectorA, glm::vec3 vectorB, float x)
{
	return glm::vec3(0.0f);
}
