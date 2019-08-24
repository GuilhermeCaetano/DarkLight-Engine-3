#ifndef _nConvert_HG_
#define _nConvert_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <LinearMath\btVector3.h>
#include <LinearMath\btTransform.h>

namespace nPhysics
{
	namespace nConvert
	{
		// Converts a vec3 from bullet to simple
		inline glm::vec3 ToSimple(const btVector3& vec)
		{
			return glm::vec3(vec.x(), vec.y(), vec.z());
		}

		inline void ToSimple(const btVector3& vecIn, glm::vec3& vecOut)
		{
			vecOut.x = vecIn.x();
			vecOut.y = vecIn.y();
			vecOut.z = vecIn.z();
		}

		inline btVector3 ToBullet(const glm::vec3& vec)
		{
			return btVector3(vec.x, vec.y, vec.z);
		}

		inline void ToBullet(const glm::vec3& vecIn, btVector3& vecOut)
		{
			vecOut.setValue(vecIn.x, vecIn.y, vecIn.z);
		}

		inline glm::mat4 ToSimple(const btTransform& transform)
		{
			glm::mat4 mat;
			transform.getOpenGLMatrix(&mat[0][0]);
			return mat;
		}

		inline void ToSimple(const btTransform& transformIn, glm::mat4& transformOut)
		{
			transformIn.getOpenGLMatrix(&transformOut[0][0]);
		}

		inline void ToBullet(const glm::quat& quatIn, btQuaternion& quatOut)
		{
			quatOut.setW(quatIn.w);
			quatOut.setX(quatIn.x);
			quatOut.setY(quatIn.y);
			quatOut.setZ(quatIn.z);
		}

		inline btQuaternion ToBullet(const glm::quat& quat)
		{
			btQuaternion quatOut;
			quatOut.setW(quat.w);
			quatOut.setX(quat.x);
			quatOut.setY(quat.y);
			quatOut.setZ(quat.z);

			return quatOut;
		}

		inline glm::quat ToSimple(const btQuaternion btQuat)
		{
			glm::quat quatOut;
			quatOut.w = btQuat.getW();
			quatOut.x = btQuat.getX();
			quatOut.y = btQuat.getY();
			quatOut.z = btQuat.getZ();

			return quatOut;
		}

		inline btTransform ToBullet(const glm::mat4& inTransform)
		{
			btTransform transformOut;
			transformOut.setFromOpenGLMatrix(&inTransform[0][0]);

			return transformOut;
		}

		inline float ToSimple(const btScalar& scalar)
		{
			float floatOut = scalar;
			return floatOut;
		}
	}
}



#endif // !_nConvert_HG_

