#ifndef _nPhysicsHelper_HG_
#define _nPhysicsHelper_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Mesh\sModelDrawInfo.h>
#include <vector>
#include <Entity\Components\iComponent.h>

namespace nPhysicsHelper
{
	void CalculateClosestPointsOnMesh(sModelDrawInfo theMeshDrawInfo, glm::vec3 pointToTest, std::vector<glm::vec3> &vecPoints);
	void GetClosestTriangle(unsigned int finalIndex, const sModelDrawInfo& info, glm::vec3& closestTriangle);
	unsigned int FindClosestPointOfAll(const glm::vec3& pointToTest, const std::vector<glm::vec3>& vecPoints, glm::vec3& finalPoint);

	void PaintClosestTriangle(sModelDrawInfo meshToPaint, unsigned int finalTriangleIndex);

	float Clamp(float val, float min, float max);
	float ClampMax(float val, float max);
	float ClampMin(float val, float min);

	bool CheckPlaneSphereCollision(iComponent* sphere, glm::vec3 moveVector, iComponent* plane, float& time, glm::vec3& collisionPoint);
	bool CheckSphereSphereCollision(iComponent* rigidbodyC1, iComponent* rigidbodyC2);

}


#endif // !_PhysicsHelper_HG_
