// nCollide.cpp

#include <SimplePhysics\Physics\nCollide.h>


int nCollide::CollideMovingSphereSphere(const glm::vec3& lastPosA, const glm::vec3& moveVectorA, float radiusA, const glm::vec3& lastPosB, const glm::vec3& moveVectorB, float radiusB, float& contactTime)
{
	// Get vector between the centers and the average movement vector
	glm::vec3 vecCenters = lastPosB - lastPosA;
	glm::vec3 vecMovement = moveVectorB - moveVectorA;

	// Get total radius of the spheres
	float totalRadius = radiusA + radiusB;

	// Spheres are already colliding if the center distance is lower than the radii
	float distanceCR = glm::dot(vecCenters, vecCenters) - (totalRadius * totalRadius);
	if (distanceCR < 0.0f)
	{
		contactTime = 0.0f;
		return -1;
	}

	float dotMove = glm::dot(vecMovement, vecMovement);
	if (dotMove < DBL_EPSILON)  // Sphere's are not moving relative to each other
	{
		return 0;
	}

	float dotMoveCenter = glm::dot(vecMovement, vecCenters);
	if (dotMoveCenter >= 0.0f) // Sphere's are not moving towards each other
	{
		return 0;
	}

	float numRoots = (dotMoveCenter * dotMoveCenter) - (dotMove * distanceCR);
	if (numRoots < 0.0f) // No real valued root, spheres will not intersect
	{
		return 0;
	}

	contactTime = (-dotMoveCenter - glm::sqrt(numRoots)) / dotMove;
	//collisionPoint = sphere0->locationC->position + (time * moveVector) - (r * planeNormal);

	if (contactTime <= 1.0f && contactTime >= 0.0f)
	{
		return 1;
	}
	else
	{
		return 0;
	}

	return 0;
}
