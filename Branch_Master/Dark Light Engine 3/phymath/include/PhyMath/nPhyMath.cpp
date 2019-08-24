// nPhyMath.cpp

#include <PhyMath\nPhyMath.h>

namespace nPhyMath
{
	EXTERN_DLL_EXPORT float nPhyMath::SquareMagnitude(glm::vec3 vec)
	{
		// Square each vector value and add them
		float retValue = (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);

		return retValue;
	}

	// The Quake fast inverse square root uses the mantissa from the float point to guess
	// a value in the division to get the square root, without actually doing the division, just bitshifting.
	// The passes then approximate the value to the real value of the square root.
	EXTERN_DLL_EXPORT float QuakeFastInvSquareRoot(float val)
	{
		float valueHalf = val * 0.5f;

		int i = *(int*)&val; // Stores the whole integer points of the value
		i = 0x5f3759df - (i >> 1); // Initial guess for Newton's method, using the whole number
		val = *(float*)&i; // Convert the guessed initial into float
		val = val * (1.5f - valueHalf * val * val); // One pass of Newtons method, so we approximate more the real value (one is around 88%)
		val = val * (1.5f - valueHalf * val * val); // Second pass. Gives around 98%, so the second might be overkill

		return val;
	}



	EXTERN_DLL_EXPORT void NormalizeVector(glm::vec3& vecToNormalize)
	{
		// Find the magnitude squared, without square root
		float squareMag = SquareMagnitude(vecToNormalize);

		// Do the fast inverse square root
		float fastInvSqrt = QuakeFastInvSquareRoot(squareMag);

		// Multiply values to get the normalized vector
		glm::vec3 normVector = vecToNormalize * fastInvSqrt;

		// We can alter the original vector,
		// there's no use keeping the unaltered one
		vecToNormalize = normVector;
	}

	EXTERN_DLL_EXPORT void GetRelativeAxis(glm::vec3 axis, glm::quat orientation, glm::vec3& outAxis)
	{
		// Update the direction vector using the quaternion orientation
		glm::mat4 matRotation = glm::mat4(orientation);

		// Temporary transform to vec4
		glm::vec4 tempFrontOfObject = glm::vec4(axis, 1.0f);

		// Update axis from orientation
		glm::vec4 newDirection = matRotation * tempFrontOfObject;

		glm::vec3 newDirection3 = glm::vec3(newDirection);
		// Normalize
		newDirection3 = glm::normalize(newDirection3);
		
		// We can't alter the original axis by reference here,
		// it could destroy the original value if not used carefully
		outAxis = newDirection3;
	}

	EXTERN_DLL_EXPORT float DotVectors(glm::vec3 vec1, glm::vec3 vec2)
	{
		float X = vec1.x * vec2.x;
		float Y = vec1.y * vec2.y;
		float Z = vec1.z * vec2.z;

		float dot = X + Y + Z;
		return dot;
	}

	EXTERN_DLL_EXPORT float GetAngleBetweenVectors(glm::vec3 vec1, glm::vec3 vec2, bool inDegrees)
	{
		// Dot the two vectors to find the relation between them
		float dotVecs = DotVectors(vec1, vec2);

		// Calculate the angle
		float angle = glm::acos(dotVecs);

		if (inDegrees)
		{
			return angle * 180 / 3.1415;
		}
		else
		{
			return angle;
		}
	}
}
