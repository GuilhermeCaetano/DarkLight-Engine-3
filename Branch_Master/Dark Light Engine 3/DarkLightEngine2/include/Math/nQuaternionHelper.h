#ifndef _nQuaternionHelper_HG_
#define _nQuaternionHelper_HG_

#define GLM_ENABLE_EXPERIMENTAL		// To get glm quaternion stuff to compile
#include <glm/gtx/quaternion.hpp>	// Note strange folder

namespace nQuaternionHelper
{
	// Set mesh orientation using vec3
	glm::quat SetOrientationEulerAngles(glm::vec3 newEulerAngle, bool bIsDegrees);

	// Set mesh orientation using xyx
	glm::quat SetOrientationEulerAngles(float x, float y, float z, bool bIsDegrees);

	// Adjust mesh orientation using vec3
	glm::quat AdjOrientationEulerAngles(glm::quat quatToAdjust, glm::vec3 adjEulerAngle, bool bIsDegrees);

	// Adjust mesh orientation using xyz
	glm::quat AdjOrientationEulerAngles(glm::quat quatToAdjust, float x, float y, float z, bool bIsDegrees);

	// Adjust mesh orientation using quaternions
	glm::quat AdjOrientationQuat(glm::quat quatToAdjust, glm::quat adjOrientQ);

	// Creates a quaternion from 2 vectors
	static glm::quat RotateTowards(glm::vec3 vectorA, glm::vec3 vectorB);

	// Rotate a vector to smoothly point in the same direction as another
	//glm::quat Slerp(glm::quat fromRotation, glm::quat toRotation);
	//glm::vec3 SlerpTowards(glm::vec3 VectorA, glm::vec3 vectorB, float x);
};


#endif // !_cQuaternions_HG_

