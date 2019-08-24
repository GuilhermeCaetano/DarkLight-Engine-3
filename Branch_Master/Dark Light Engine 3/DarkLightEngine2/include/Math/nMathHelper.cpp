// nMathHelper.cpp

#include <Math\nMathHelper.h>



namespace nMathHelper
{
	// The Quake fast inverse square root uses the mantissa from the float point to guess
	// a value in the division to get the square root, without actually doing the division, just bitshifting.
	// The passes then approximate the value to the real value of the square root.
	float QuakeFastInvSquareRoot(float val)
	{
		float valueHalf = val * 0.5f;

		int i = *(int*)&val; // Stores the whole integer points of the value
		i = 0x5f3759df - (i >> 1); // Initial guess for Newton's method, using the whole number
		val = *(float*)&i; // Convert the guessed initial into float
		val = val * (1.5f - valueHalf * val * val); // One pass of Newtons method, so we approximate more the real value (one is around 88%)
		val = val * (1.5f - valueHalf * val * val); // Second pass. Gives around 98%, so the second might be overkill

		return val;
	}

	float SquareMagnitude(glm::vec3 vec)
	{
		// Square each vector value and add them
		float retValue = (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);

		return retValue;
	}

	glm::vec3 NormalizeVector(glm::vec3 vecToNormalize)
	{
		// Find the magnitude squared, without square root
		float squareMag = SquareMagnitude(vecToNormalize);

		// Do the fast inverse square root
		float fastInvSqrt = QuakeFastInvSquareRoot(squareMag);

		// Multiply values to get the normalized vector
		glm::vec3 normVector = vecToNormalize * fastInvSqrt;

		return normVector;
	}

	glm::vec3 GetRelativeAxis(glm::vec3 axis, glm::quat orientation)
	{
		// Update the direction vector using the quaternion orientation
		glm::mat4 matRotation = glm::mat4(orientation);

		// Temporary transform to vec4
		glm::vec4 tempFrontOfObject = glm::vec4(axis, 1.0f);

		// Update axis from orientation
		glm::vec4 newDirection = matRotation * tempFrontOfObject;

		// Normalize
		glm::vec3 normDirection = NormalizeVector(glm::vec3(newDirection));

		return normDirection;
	}

	float DotVectors(glm::vec3 vec1, glm::vec3 vec2)
	{
		float X = vec1.x * vec2.x;
		float Y = vec1.y * vec2.y;
		float Z = vec1.z * vec2.z;

		float dot = X + Y + X;
		return dot;
	}

	
	float Smoothstep(float edge0, float edge1, float x)
	{
		// Scale, bias and saturate x to 0..1 range
		x = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		// Evaluate polynomial
		return (x * x * 3.0f) - (2.0f * x);
	}

	float Smootherstep(float edge0, float edge1, float x)
	{
		// Scale, and clamp x to 0..1 range
		x = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		// Evaluate polynomial
		return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
	}

	float Clamp(float val, float min, float max)
	{
		val = ClampMax(val, max);
		val = ClampMin(val, min);

		return val;
	}

	float ClampMax(float val, float max)
	{
		if (val >= max) { val = max; }
		return val;
	}

	float ClampMin(float val, float min)
	{
		if (val <= min) { val = min; }
		return val;
	}
}
