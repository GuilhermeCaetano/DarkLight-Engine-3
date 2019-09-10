// PhysicsHelper.cpp

#include <Helpers\nPhysicsHelper.h>
#include <SimplePhysics\Physics\sSimpleRigidbodyComponent.h>
#include <Interfaces\Physics\iPlaneShape.h>
#include <Interfaces\Physics\iSphereShape.h>
#include <Entity\Components\sLocationComponent.h>

namespace nPhysicsHelper
{
	typedef glm::vec3 Point;
	typedef glm::vec3 Vector;

	unsigned int paintedVertices[3] = { NULL, NULL, NULL };

	// Calculates the closest point on a triangle, given p(point to check), and a,b,c(triangle points)
	// Returns the closest point to p on the triangle ABC.
	Point ClosestPtPointTriangle(Point p, Point a, Point b, Point c)
	{
		Vector ab = b - a;
		Vector ac = c - a;
		Vector bc = c - b;

		// Compute parametric position s for projection P' of P on AB,
		// P' = A + s*AB, s = snom/(snom+sdenom)
		//   float snom = Dot(p - a, ab), sdenom = Dot(p - b, a - b);
		float snom = glm::dot(p - a, ab);
		float sdenom = glm::dot(p - b, a - b);

		// Compute parametric position t for projection P' of P on AC,
		// P' = A + t*AC, s = tnom/(tnom+tdenom)
		//    float tnom = Dot(p - a, ac), tdenom = Dot(p - c, a - c);
		float tnom = glm::dot(p - a, ac);
		float tdenom = glm::dot(p - c, a - c);

		if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

													// Compute parametric position u for projection P' of P on BC,
													// P' = B + u*BC, u = unom/(unom+udenom)
													//    float unom = Dot(p - b, bc), udenom = Dot(p - c, b - c);
		float unom = glm::dot(p - b, bc);
		float udenom = glm::dot(p - c, b - c);

		if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
		if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out


														// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
														//    Vector n = Cross(b - a, c - a);
		Vector n = glm::cross(b - a, c - a);
		//    float vc = Dot(n, Cross(a - p, b - p));

		float vc = glm::dot(n, glm::cross(a - p, b - p));

		// If P outside AB and within feature region of AB,
		// return projection of P onto AB
		if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
			return a + snom / (snom + sdenom) * ab;

		// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
		//    float va = Dot(n, Cross(b - p, c - p));
		float va = glm::dot(n, glm::cross(b - p, c - p));


		// If P outside BC and within feature region of BC,
		// return projection of P onto BC
		if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
			return b + unom / (unom + udenom) * bc;

		// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
		//    float vb = Dot(n, Cross(c - p, a - p));
		float vb = glm::dot(n, glm::cross(c - p, a - p));

		// If P outside CA and within feature region of CA,
		// return projection of P onto CA
		if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
			return a + tnom / (tnom + tdenom) * ac;

		// P must project inside face region. Compute Q using barycentric coordinates
		float u = va / (va + vb + vc);
		float v = vb / (va + vb + vc);
		float w = 1.0f - u - v; // = vc / (va + vb + vc)
		return u * a + v * b + w * c;
	}

	// Calculates the closest points on a mesh given the point to test and the mesh to be tested with
	// Returns a vector of points closest to the test point on the mesh by reference.
	void CalculateClosestPointsOnMesh(sModelDrawInfo theMeshDrawInfo, glm::vec3 pointToTest, std::vector<glm::vec3>& vecPoints)
	{
		vecPoints.clear();

		// For each triangle in the mesh information...
		for (unsigned int triIndex = 0; triIndex != theMeshDrawInfo.numberOfTriangles; triIndex++)
		{
			sMeshTriangle CurTri = theMeshDrawInfo.originalMeshInfo->pTrianglesFromFile[triIndex];

			// ... call the ClosestPointToTriangle...
			// Need to get the 3 vertices of the triangle
			sMeshVertex corner_1 = theMeshDrawInfo.originalMeshInfo->pVerticesFromFile[CurTri.vertex1];
			sMeshVertex corner_2 = theMeshDrawInfo.originalMeshInfo->pVerticesFromFile[CurTri.vertex2];
			sMeshVertex corner_3 = theMeshDrawInfo.originalMeshInfo->pVerticesFromFile[CurTri.vertex3];

			// Convert this to glm::vec3
			glm::vec3 vert_1 = glm::vec3(corner_1.x, corner_1.y, corner_1.z);
			glm::vec3 vert_2 = glm::vec3(corner_2.x, corner_2.y, corner_2.z);
			glm::vec3 vert_3 = glm::vec3(corner_3.x, corner_3.y, corner_3.z);

			glm::vec3 closestPoint = ClosestPtPointTriangle(pointToTest, vert_1, vert_2, vert_3);

			vecPoints.push_back(closestPoint);
		}
	}

	// Calculates the closest triangle on a mesh given the index from the closest point of all
	void GetClosestTriangle(unsigned int finalIndex, const sModelDrawInfo& info, glm::vec3& closestTriangle)
	{
		// By logic, the closest point index is the closest triangle index.
		int x, y, z;
		x = info.originalMeshInfo->pTrianglesFromFile[finalIndex].vertex1;
		y = info.originalMeshInfo->pTrianglesFromFile[finalIndex].vertex2;
		z = info.originalMeshInfo->pTrianglesFromFile[finalIndex].vertex3;

		// Update the closest triangle
		closestTriangle = { x, y, z };
	}

	// Calculates the closest point to another given a vector of points to test.
	// Returns the closest point by reference and the index of the triangle the point is in.
	unsigned int FindClosestPointOfAll(const glm::vec3& pointToTest, const std::vector<glm::vec3>& vecPoints, glm::vec3& finalPoint)
	{
		glm::vec3 closestPoint = vecPoints[0]; // Begin assuming that the closes point is the first one
		float currentDist = glm::distance(pointToTest, closestPoint); // Check the distance with first assumed point

		unsigned int finalIndex = 0;
		for (unsigned int index = 1; index != vecPoints.size(); index++)
		{
			// Check next point distance
			// Should we set it to absolute?
			float nextDist = glm::distance(pointToTest, vecPoints[index]);
			if (nextDist < currentDist)
			{
				// If the distance checked now is smaller, this is the current smallest distanced point
				currentDist = nextDist;
				// And this is then our new closest point
				closestPoint = vecPoints[index];
				finalIndex = index;
			}
		}

		// In the end, pass the new closest point
		finalPoint = closestPoint;

		return finalIndex;
	}


	void PaintClosestTriangle(sModelDrawInfo meshToPaint, unsigned int finalTriangleIndex)
	{
		if (paintedVertices[0] != NULL)
		{
			// Not first time starting, clean the last attempt
			// and reset the color to white
			for (int i = 0; i != 3; i++)
			{
				meshToPaint.pVerticesToVBO[paintedVertices[i]].r = 1.0f;
				meshToPaint.pVerticesToVBO[paintedVertices[i]].g = 1.0f;
				meshToPaint.pVerticesToVBO[paintedVertices[i]].b = 1.0f;
			}
		}

		unsigned int v1 = meshToPaint.originalMeshInfo->pTrianglesFromFile[finalTriangleIndex].vertex1;
		unsigned int v2 = meshToPaint.originalMeshInfo->pTrianglesFromFile[finalTriangleIndex].vertex2;
		unsigned int v3 = meshToPaint.originalMeshInfo->pTrianglesFromFile[finalTriangleIndex].vertex3;

		paintedVertices[0] = v1;
		paintedVertices[1] = v2;
		paintedVertices[2] = v3;

		for (int i = 0; i != 3; i++)
		{
			meshToPaint.pVerticesToVBO[paintedVertices[i]].r = 1.0f;
			meshToPaint.pVerticesToVBO[paintedVertices[i]].g = 0.0f;
			meshToPaint.pVerticesToVBO[paintedVertices[i]].b = 0.0f;
		}
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

	bool CheckPlaneSphereCollision(iComponent* sphere, glm::vec3 moveVector, iComponent* plane, float& time, glm::vec3& collisionPoint)
	{
		return false;
		//nPhysics::sSimpleRigidbodyComponent* rbSphere = static_cast<nPhysics::sSimpleRigidbodyComponent*>(sphere);
		//nPhysics::sSimpleRigidbodyComponent* rbPlane = static_cast<nPhysics::sSimpleRigidbodyComponent*>(plane);

		//nPhysics::iPlaneShape* iPlane = static_cast<nPhysics::iPlaneShape*>(rbPlane->shape);
		//nPhysics::iSphereShape* iSphere = static_cast<nPhysics::iSphereShape*>(rbSphere->shape);

		//sLocationComponent* sphereLocation = static_cast<sLocationComponent*>(rbPlane->location);

		//// Compute distance of sphere center to plane
		//float distance = glm::dot(iPlane->GetPlaneNormal(), sphereLocation->position) - iPlane->GetPlaneConstant();
		//if (abs(distance) <= iSphere->GetRadius())
		//{
		//	// The sphere is already overlapping the plane.
		//	// Set time of intersection to zero and the return point to the sphere center.
		//	time = 0.0f;
		//	collisionPoint = sphereLocation->position;
		//	return true;
		//}
		//else
		//{
		//	float denom = glm::dot(iPlane->GetPlaneNormal(), moveVector);
		//	if (denom * distance >= 0.0f)
		//	{
		//		// no intersection as the sphere is moving parallel to or away from the plane
		//		return false;
		//	}
		//	else
		//	{
		//		// Sphere is moving towards the plane

		//		// use +r in computations if sphere in front of plane, else -r
		//		float r = distance > 0.0f ? iSphere->GetRadius() : -iSphere->GetRadius();
		//		time = (r - distance) / denom;
		//		collisionPoint = sphereLocation->position + (time * moveVector) - (r * iPlane->GetPlaneNormal());
		//		return true;
		//	}
		//}
	}

	bool CheckSphereSphereCollision(iComponent* rigidbodyC1, iComponent* rigidbodyC2)
	{
		return false;
	}
}
