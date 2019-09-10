#ifndef _nMathHelper_HG_
#define _nMathHelper_HG_


#include <OpenGL\OpenGLHeaders.h>


namespace nMathHelper
{
	float SquareMagnitude(glm::vec3 vec);

	float QuakeFastInvSquareRoot(float v);
	//float SquareMagnitude(glm::vec3 vec);
	float DotVectors(glm::vec3 vec1, glm::vec3 vec2);
	//float AcosVectors();

	glm::vec3 GetRelativeAxis(glm::vec3 forward, glm::quat orientation);
	glm::vec3 NormalizeVector(glm::vec3 vecToNormalize);

	float Smoothstep(float edge0, float edge1, float x);
	float Smootherstep(float edge0, float edge1, float x);

	float Clamp(float val, float min, float max);
	float ClampMax(float val, float max);
	float ClampMin(float val, float min);

	// Inspired by: https://stackoverflow.com/questions/686353/c-random-float-number-generation
	template <class T>
	T GetRandInRange(T min, T max)
	{
		double value = min + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (static_cast<double>(max - min))));
		return static_cast<T>(value);
	}
}


#endif // !_nMathHelper_HG_
