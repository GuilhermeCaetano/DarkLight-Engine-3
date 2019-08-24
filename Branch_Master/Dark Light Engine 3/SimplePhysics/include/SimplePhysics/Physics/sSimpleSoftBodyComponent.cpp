// sSimpleSoftBodyComponent.cpp

#include <SimplePhysics\Physics\sSimpleSoftBodyComponent.h>
#include <SimplePhysics\Physics\cSphereShape.h>
#include <SimplePhysics\Physics\nCollide.h>
#include <algorithm>


namespace nPhysics
{
	sSimpleSoftBodyComponent::sSimpleSoftBodyComponent(const sSoftbodyDef& def, unsigned long long entityID)
	{
		//this->transform = static_cast<sLocationComponent*>(transformC);

		this->springConstant = def.springConstant;
		this->numNodesAB = def.numNodesAB;
		this->numNodesAC = def.numNodesAC;
		this->cornerA = glm::vec3(def.cornerA[0], def.cornerA[1], def.cornerA[2]);
		this->cornerB = glm::vec3(def.cornerB[0], def.cornerB[1], def.cornerB[2]);
		this->cornerC = glm::vec3(def.cornerC[0], def.cornerC[1], def.cornerC[2]);

		this->_windYaw = 0.0f;
		this->_windPitch = 1.0f;
		this->_windForce = 0.0f;
		this->_windDirection = glm::vec3(this->_windYaw, 0.0f, this->_windPitch);
		this->_windMagnitude = glm::length(this->_windDirection * this->_windForce);

		this->_GenerateSoftbody();

		return;
	}

	sSimpleSoftBodyComponent::~sSimpleSoftBodyComponent()
	{
		this->Cleanup();

		return;
	}

	void sSimpleSoftBodyComponent::Start()
	{
	}

	glm::mat4 sSimpleSoftBodyComponent::GetTransform()
	{
		return glm::mat4();
	}

	void sSimpleSoftBodyComponent::SetTransform(const glm::mat4 & transform)
	{
	}

	sLocationComponent* sSimpleSoftBodyComponent::GetTransformComponent()
	{
		return this->_transform;
	}

	void sSimpleSoftBodyComponent::SetTransformComponent(sLocationComponent* locationC)
	{
		this->_transform = locationC;
	}

	eCollisionLayerMask sSimpleSoftBodyComponent::GetCollisionLayerMask()
	{
		return this->_collisionLayerMask;
	}

	void sSimpleSoftBodyComponent::SetCollisionLayerMask(eCollisionLayerMask layerMask)
	{
		this->_collisionLayerMask = layerMask;
	}

	void sSimpleSoftBodyComponent::_GenerateSoftbody()
	{
		// Generate nodes
		glm::vec3 position;
		glm::vec3 distanceAB = glm::vec3((cornerB - cornerA) / float(numNodesAB - 1));
		glm::vec3 distanceAC = glm::vec3((cornerC - cornerA) / float(numNodesAC - 1));
		for (unsigned int i = 0; i < this->numNodesAC; i++)
		{
			for (unsigned int j = 0; j < this->numNodesAB; j++)
			{
				position = glm::vec3(cornerA + distanceAB * (float)j + distanceAC * (float)i);
				this->_vecNodes.push_back(new cNode(position, this->GetTransformComponent()->GetWorldPosition(), 1.0f));
			}
		}
		for (unsigned int i = 0; i < numNodesAB; i++)
		{
			this->_vecNodes[i]->mass = 0.0f;
		}

		// Generate springs
		for (unsigned int i = 0; i < this->numNodesAC - 1; i++) // Vertical
		{
			for (unsigned int j = 0; j < this->numNodesAB - 1; j++) // Horizontal
			{
				unsigned int index = j + i * this->numNodesAB;
				cNode* nodeA = this->_vecNodes[index];
				cNode* nodeB = this->_vecNodes[index + 1];
				cNode* nodeC = this->_vecNodes[index + numNodesAB];

				if (!nodeA->HasNeighbour(nodeB))
				{
					cSpring* spring = new cSpring(nodeA, nodeB, this->springConstant);
					nodeA->vecSprings.push_back(spring);
					nodeB->vecSprings.push_back(spring);
					this->_vecSprings.push_back(spring);
				}

				if (!nodeA->HasNeighbour(nodeC))
				{
					cSpring* spring = new cSpring(nodeA, nodeC, this->springConstant);
					nodeA->vecSprings.push_back(spring);
					nodeC->vecSprings.push_back(spring);
					this->_vecSprings.push_back(spring);
				}
			}
		}

		unsigned int numNodes = this->numNodesAB * this->numNodesAC;
		for (unsigned int i = 0; i < numNodesAB; i++) // Last horizontal row
		{
			unsigned int index = numNodes - i - 1;
			cNode* nodeA = this->_vecNodes[index];
			cNode* nodeB = this->_vecNodes[index - 1];

			if (!nodeA->HasNeighbour(nodeB))
			{
				cSpring* spring = new cSpring(nodeA, nodeB, this->springConstant);
				nodeA->vecSprings.push_back(spring);
				nodeB->vecSprings.push_back(spring);
				this->_vecSprings.push_back(spring);
			}
		}
		for (unsigned int i = 1; i < numNodesAC; i++) // Last vertical row
		{
			unsigned int index = (numNodesAB * i) - 1;
			cNode* nodeA = this->_vecNodes[index];
			cNode* nodeB = this->_vecNodes[index + numNodesAB];

			if (!nodeA->HasNeighbour(nodeB))
			{
				cSpring* spring = new cSpring(nodeA, nodeB, this->springConstant);
				nodeA->vecSprings.push_back(spring);
				nodeB->vecSprings.push_back(spring);
				this->_vecSprings.push_back(spring);
			}
		}

		for (unsigned int i = 0; i < this->_vecNodes.size(); i++)
		{
			this->_vecNodes[i]->ComputeRadius();
		}
	}

	unsigned int sSimpleSoftBodyComponent::GetNumNodes()
	{
		return this->_vecNodes.size();
	}

	glm::vec3 sSimpleSoftBodyComponent::GetNodePosition(unsigned int index)
	{
		return this->_vecNodes[index]->position;
	}

	float sSimpleSoftBodyComponent::GetNodeRadius(unsigned int index)
	{
		return this->_vecNodes[index]->radius;
	}

	void sSimpleSoftBodyComponent::GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut)
	{
		minBoundsOut = this->_minBounds;
		maxBoundsOut = this->_maxBounds;
	}

	void sSimpleSoftBodyComponent::UpdateInternal(double deltaTime, const glm::vec3& gravity)
	{
		cNode* node = nullptr;
		for (unsigned int i = 0; i < this->_vecNodes.size(); i++)
		{
			// Set positions
			node = this->_vecNodes[i];
			glm::vec3 parentDif = this->GetTransformComponent()->GetWorldPosition() - node->parentPosition;
			node->parentPosition = this->GetTransformComponent()->GetWorldPosition();
			node->position += parentDif;

			this->_vecNodes[i]->springForce = gravity * this->_vecNodes[i]->mass;
		}
		for (unsigned int i = 0; i < this->_vecSprings.size(); i++)
		{
			this->_vecSprings[i]->ApplyForce();
		}
		for (unsigned int i = 0; i < this->_vecNodes.size(); i++)
		{
			// Apply wind and integrate
			this->_windDirection = glm::vec3(this->_windYaw, 0.0f, this->_windPitch) * this->_windForce;
			this->_vecNodes[i]->velocity += this->_windDirection;
			this->_vecNodes[i]->Integrate(deltaTime);
		}
		for (unsigned int idxA = 0; idxA < this->_vecNodes.size() - 1; idxA++)
		{
			for (unsigned int idxB = idxA + 1; idxB < this->_vecNodes.size(); idxB++)
			{
				this->_CollideNodes(this->_vecNodes[idxA], this->_vecNodes[idxB], deltaTime);
			}
		}

		this->_UpdateAABB();
	}

	void sSimpleSoftBodyComponent::Cleanup()
	{
		for (unsigned int i = 0; i < this->_vecNodes.size(); i++)
		{
			delete this->_vecNodes[i];
		}
		this->_vecNodes.clear();
		for (unsigned int i = 0; i < this->_vecSprings.size(); i++)
		{
			delete this->_vecSprings[i];
		}
		this->_vecSprings.clear();
	}

	sSimpleSoftBodyComponent::cSpring::cSpring(cNode * newNodeA, cNode * newNodeB, float newSpringconstant)
	{
		this->nodeA = newNodeA;
		this->nodeB = newNodeB;
		this->springConstant = newSpringconstant;
		this->restingDistance = glm::length(this->nodeA->position - this->nodeB->position);
	}

	void sSimpleSoftBodyComponent::cSpring::ApplyForce()
	{
		// Calculate separation between nodes
		glm::vec3 separationAmount = this->nodeB->position - this->nodeA->position;
		float currentDistance = glm::length(separationAmount);

		// Calculate and apply force to be applied due to distance
		glm::vec3 force = (separationAmount / currentDistance) * (-this->springConstant * (currentDistance - this->restingDistance));
		this->nodeA->springForce -= force;
		this->nodeB->springForce += force;
	}

	sSimpleSoftBodyComponent::cNode* sSimpleSoftBodyComponent::cSpring::GetOtherNode(cNode* node)
	{
		return node == this->nodeA ? nodeB : nodeA;
	}

	sSimpleSoftBodyComponent::cNode::cNode(const glm::vec3& newPosition, const glm::vec3& newParentPos, float newMass)
	{
		this->position = newPosition + newParentPos;
		this->parentPosition = newParentPos;
		this->localPosition = newPosition;
		this->lastPosition = this->position;
		this->mass = newMass;
		this->springForce = glm::vec3(0.0f);
		this->velocity = glm::vec3(0.0f);
	}

	sSimpleSoftBodyComponent::cNode::~cNode()
	{
		delete this->sphere;
	}

	bool sSimpleSoftBodyComponent::cNode::HasNeighbour(cNode* node)
	{
		return this->vecSprings.end() != std::find_if(this->vecSprings.begin(), this->vecSprings.end(), [node, this](cSpring* spring)
		{
			return node == spring->GetOtherNode(this);
		});
	}

	void sSimpleSoftBodyComponent::cNode::ComputeRadius()
	{
		float closestNeighbour = std::numeric_limits<float>::max();
		for (unsigned int i = 0; i < this->vecSprings.size(); i++)
		{
			float distance = glm::length(this->vecSprings[i]->GetOtherNode(this)->position - this->position);
			closestNeighbour = glm::min(closestNeighbour, distance);
		}
		this->radius = 0.45f * closestNeighbour;
		this->sphere = new cSphereShape(this->radius);
	}

	void sSimpleSoftBodyComponent::cNode::Integrate(double deltaTime)
	{
		// Update last position to avoid tunneling
		this->lastPosition = this->position;

		// Fixed nodes won't move
		if (this->IsFixed())
		{
			return;
		}

		// Integrate
		this->velocity += springForce * (float)deltaTime / this->mass;
		this->position += velocity * (float)deltaTime;

		// Damping
		float damping = 0.9f;
		this->velocity *= glm::pow(1.0f - damping, deltaTime);
	}

	void sSimpleSoftBodyComponent::_CollideNodes(cNode* nodeA, cNode* nodeB, double deltaTime)
	{
		// We don't want nodes that are already
		// interacting via spring to collide with each other
		if (nodeA->HasNeighbour(nodeB))
		{
			return;
		}

		glm::vec3 moveVectorA = nodeA->position - nodeA->lastPosition;
		glm::vec3 moveVectorB = nodeB->position - nodeB->lastPosition;

		float contactTime = 0.0f;
		int result = nCollide::CollideMovingSphereSphere(nodeA->lastPosition, moveVectorA, nodeA->radius, nodeB->lastPosition, moveVectorB, nodeB->radius, contactTime);

		if (result == -1) // Nodes are already colliding
		{
			// point in the middle as collision point
			glm::vec3 closestPoint = (nodeA->lastPosition + nodeB->lastPosition) * 0.5f;

			// Point + normalize
			nodeA->position = closestPoint + glm::normalize(nodeA->lastPosition - nodeB->lastPosition) * (nodeA->radius + 0.5f);
			nodeB->position = closestPoint + glm::normalize(nodeB->lastPosition - nodeA->lastPosition) * (nodeB->radius + 0.5f);
		}
		else if(result == 1)
		{
			float bounceAmount = 1.0f;

			// Go back in position in time
			if (!nodeA->IsFixed())
			{
				nodeA->position = nodeA->lastPosition + moveVectorA * contactTime;
				//nodeA->relativePosition = nodeA->position + this->transform->position;
			}
			if (!nodeB->IsFixed())
			{
				nodeB->position = nodeB->lastPosition + moveVectorB * contactTime;
				//nodeB->relativePosition = nodeB->position + this->transform->position;
			}

			// Apply elastic velocity
			nodeA->velocity = (bounceAmount * nodeB->mass * (moveVectorB - moveVectorA) + nodeA->mass * moveVectorA + nodeB->mass * moveVectorB) / (nodeA->mass + nodeB->mass);
			nodeB->velocity = (bounceAmount * nodeA->mass * (moveVectorA - moveVectorB) + nodeA->mass * moveVectorA + nodeB->mass * moveVectorB) / (nodeA->mass + nodeB->mass);

			// reintegrate
			nodeA->Integrate(deltaTime * (1.0f - contactTime));
			nodeB->Integrate(deltaTime * (1.0f - contactTime));
		}
	}

	void sSimpleSoftBodyComponent::_UpdateAABB()
	{
		this->_minBounds.x = std::numeric_limits<float>::max();
		this->_minBounds.y = std::numeric_limits<float>::max();
		this->_minBounds.z = std::numeric_limits<float>::max();

		this->_maxBounds.x = std::numeric_limits<float>::min();
		this->_maxBounds.y = std::numeric_limits<float>::min();
		this->_maxBounds.z = std::numeric_limits<float>::min();

		for (unsigned int i = 0; i < this->_vecNodes.size(); i++)
		{
			cNode* node = this->_vecNodes[i];

			this->_minBounds.x = glm::min(this->_minBounds.x, node->position.x - node->radius);
			this->_minBounds.y = glm::min(this->_minBounds.y, node->position.y - node->radius);
			this->_minBounds.z = glm::min(this->_minBounds.z, node->position.z - node->radius);

			this->_maxBounds.x = glm::max(this->_maxBounds.x, node->position.x + node->radius);
			this->_maxBounds.y = glm::max(this->_maxBounds.y, node->position.y + node->radius);
			this->_maxBounds.z = glm::max(this->_maxBounds.z, node->position.z + node->radius);
		}
	}

	void sSimpleSoftBodyComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
	{
		prettyWriter.Key("NumNodesAB"); prettyWriter.Int(this->numNodesAB);
		prettyWriter.Key("NumNodesAC"); prettyWriter.Int(this->numNodesAC);
		prettyWriter.Key("CornerA"); prettyWriter.StartArray();
		prettyWriter.Double(this->cornerA.x); prettyWriter.Double(this->cornerA.y); prettyWriter.Double(this->cornerA.z);
		prettyWriter.EndArray();
		prettyWriter.Key("CornerB"); prettyWriter.StartArray();
		prettyWriter.Double(this->cornerB.x); prettyWriter.Double(this->cornerB.y); prettyWriter.Double(this->cornerB.z);
		prettyWriter.EndArray();
		prettyWriter.Key("CornerC"); prettyWriter.StartArray();
		prettyWriter.Double(this->cornerC.x); prettyWriter.Double(this->cornerC.y); prettyWriter.Double(this->cornerC.z);
		prettyWriter.EndArray();
	}

	void sSimpleSoftBodyComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator iter)
	{
		this->numNodesAB = iter->FindMember("NumNodesAB")->value.GetInt();
		this->numNodesAC = iter->FindMember("NumNodesAC")->value.GetInt();
		rapidjson::Value::ConstArray arrA = iter->FindMember("CornerA")->value.GetArray();
		this->cornerA = glm::vec3((float)arrA[0].GetDouble(), (float)arrA[1].GetDouble(), (float)arrA[2].GetDouble());
		rapidjson::Value::ConstArray arrB = iter->FindMember("CornerB")->value.GetArray();
		this->cornerB = glm::vec3((float)arrB[0].GetDouble(), (float)arrB[1].GetDouble(), (float)arrB[2].GetDouble());
		rapidjson::Value::ConstArray arrC = iter->FindMember("CornerC")->value.GetArray();
		this->cornerC = glm::vec3((float)arrC[0].GetDouble(), (float)arrC[1].GetDouble(), (float)arrC[2].GetDouble());
		this->bodyType = BODY_TYPE_SOFTBODY;

		this->Cleanup();

		this->_GenerateSoftbody();
	}
}
