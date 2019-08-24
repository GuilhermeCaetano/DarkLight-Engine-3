#ifndef _nPhyMath_HG_
#define _nPhyMath_HG_



#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

#include <OpenGL\OpenGLHeaders.h>



namespace nPhyMath
{
	EXTERN_DLL_EXPORT float SquareMagnitude(glm::vec3 vec);

	EXTERN_DLL_EXPORT float QuakeFastInvSquareRoot(float v);
	
	EXTERN_DLL_EXPORT float DotVectors(glm::vec3 vec1, glm::vec3 vec2);

	EXTERN_DLL_EXPORT void GetRelativeAxis(glm::vec3 forward, glm::quat orientation, glm::vec3& outAxis);
	EXTERN_DLL_EXPORT void NormalizeVector(glm::vec3& vecToNormalize);

	EXTERN_DLL_EXPORT float GetAngleBetweenVectors(glm::vec3 vec1, glm::vec3 vec2, bool inDegrees);
}


#endif // !_nPhyMath_HG_
