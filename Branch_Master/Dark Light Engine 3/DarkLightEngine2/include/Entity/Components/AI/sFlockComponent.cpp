// sFlockComponent.cpp

#include <Entity\Components\AI\sFlockComponent.h>
#include <Entity\sEntity.h>


sFlockComponent::sFlockComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->Start();

	return;
}

sFlockComponent::~sFlockComponent()
{
	for (unsigned int i = 0; i < this->_vecBoids.size(); i++)
	{
		delete this->_vecBoids[i];
		this->_vecBoids[i] = nullptr;
	}

	this->_vecBoids.clear();

	return;
}

void sFlockComponent::Start()
{
	this->flockEntity = nullptr;
	this->masterFlock = nullptr;

	this->_cohesionWeight = 0.33f;
	this->_separationWeight = 0.33f;
	this->_alignmentWeight = 0.33f;

	this->_cohesion = glm::vec3(0.0f);
	this->_separation = glm::vec3(0.0f);
	this->_alignment = glm::vec3(0.0f);
	this->_steering = glm::vec3(0.0f);

	this->flockTotalSize = 0;
	this->flockCurrentSize = 0;
	this->flockRadius = 200.0f;
	this->flockSpeed = 10.0f;

	this->flockFormation = LINE;
	this->lastFlockFormation = LINE;
	this->centerPosition = glm::vec3(0.0f);
	this->offset = glm::vec3(0.0f);

	this->followPath = false;
	this->currentPathNode = 0;
	this->pathMode = 0;
}

void sFlockComponent::SetWeights(float cohesionW, float separationW, float alignmentW)
{
	if ((cohesionW + separationW + alignmentW) != 1.0f)
	{
		printf("Warning! Flock weights don't sum to 1!\n");
	}

	this->_cohesionWeight = cohesionW;
	this->_separationWeight = separationW;
	this->_alignmentWeight = alignmentW;
}

bool sFlockComponent::AddBoid(sEntity* entity)
{
	// Create boid, set and add to vector
	sBoid* newBoid = new sBoid(entity);
	this->_vecBoids.push_back(newBoid);

	return true;
}

bool sFlockComponent::RemoveBoid(sEntity* entity)
{
	for (unsigned int i = 0; i < this->_vecBoids.size(); i++)
	{
		if (this->_vecBoids[i]->entity == entity)
		{
			delete this->_vecBoids[i];
			this->_vecBoids[i] = nullptr;
			this->_vecBoids.shrink_to_fit();
		}
	}

	return true;
}

void sFlockComponent::CalculateVectors()
{
	// Calculating motion vectors
	this->_cohesion = glm::vec3(0.0f);
	this->_separation = glm::vec3(0.0f);
	this->_alignment = glm::vec3(0.0f);
	this->flockCurrentSize = 0;

	this->centerPosition = this->flockEntity->transform->GetWorldPosition();

	if (this->masterFlock != nullptr)
	{
		this->_vecBoids = this->masterFlock->_vecBoids;
	}

	for (unsigned int i = 0; i != this->_vecBoids.size(); i++)
	{
		sEntity* entity = this->_vecBoids[i]->entity;

		// Master flock only cares about cohesion
		if (this->masterFlock == nullptr)
		{
			// Cohesion: The average position of all the members of the flock
			this->_cohesion += entity->transform->GetWorldPosition();
			continue;
		}

		// Only add value to vectors if the boid is within radius
		if (fabs(glm::distance(entity->transform->GetWorldPosition(), this->centerPosition)) > this->flockRadius)
		{
			continue;
		}

		this->flockCurrentSize++;
		// Separation: The average of all normalized vectors between the boids
		if (fabs(glm::distance(this->flockEntity->transform->GetWorldPosition(), entity->transform->GetWorldPosition())) != 0.0f)
		{
			this->_separation += glm::normalize(this->flockEntity->transform->GetWorldPosition() - entity->transform->GetWorldPosition());
		}

		// Alignment: The average forward of all the other boids on the flock
		this->_alignment += entity->transform->GetForwardVector();
	}

	if (this->_separation.length() != 0.0f)
	{
		this->_separation = glm::normalize(this->_separation);
	}
}

void sFlockComponent::CalculateSteering()
{
	// Update vectors
	this->CalculateVectors();
	

	if (this->masterFlock == nullptr)
	{
		return;
	}

	this->masterFlock->CalculateVectors();

	/*if (this->flockCurrentSize < 2)
	{
		this->_steering = this->flockEntity->transform->direction;
		return;
	}*/

	this->_cohesion = this->masterFlock->CalculateCohesionFor(this->flockEntity);
	this->_separation = this->CalculateSeparationFor(this->flockEntity);
	this->_alignment = this->CalculateAlignmentFor(this->flockEntity);

	this->_steering = this->_cohesion + this->_separation + this->_alignment;

	if (glm::length(this->_steering) == 0.0f)
	{
		this->_steering = this->flockEntity->transform->GetForwardVector();
	}
	else
	{
		this->_steering = glm::normalize(this->_steering);
	}
}

glm::vec3 sFlockComponent::CalculateCohesionFor(sEntity* boidEntity)
{
	float flockOtherBoidsSize = this->_vecBoids.size() - 1;

	// Calculate cohesion
	// To calculate the cohesion for a specific boid, we need to remove the calculated boid position
	// from the rest. Add the center position as an anchor and add + 1 to the total flock size to account for it. Then divide by the rest amount.
	// We get the average cohesion point, but we still have to subtract the position to make it a direction rather than a point.
	glm::vec3 cohesion = (this->_cohesion - boidEntity->transform->GetWorldPosition() + this->centerPosition) / (flockOtherBoidsSize + 1.0f) - boidEntity->transform->GetWorldPosition();
	cohesion = glm::normalize(cohesion);
	cohesion *= this->_cohesionWeight;
	return cohesion;
}

glm::vec3 sFlockComponent::CalculateSeparationFor(sEntity* boidEntity)
{
	// Calculate separation
	// To calculate separation for a specific boid, we need to subtract the position to get the 
	// amount for all the other boids, then divide by the rest amount to get the average
	// and reverse the sign since it's a separation
	this->_separation *= this->_separationWeight;
	return this->_separation;
}

glm::vec3 sFlockComponent::CalculateAlignmentFor(sEntity* boidEntity)
{
	float flockOtherBoidsSize = this->flockCurrentSize - 1;
	if (flockOtherBoidsSize <= 0)
	{
		return boidEntity->transform->GetForwardVector();
	}

	// Calculate alignment
	// To calculate the alignment for a specific boid, we need to subtract the forward to get
	// the amount for all the other boids, then divide the rest amount to get the average
	glm::vec3 alignment = (this->_alignment - boidEntity->transform->GetForwardVector()) / flockOtherBoidsSize;
	alignment = glm::normalize(alignment);
	alignment *= this->_alignmentWeight;
	return alignment;
}

//glm::vec3 sFlockComponent::GetSteeringVectorFor(sEntity* boidEntity)
//{
//	glm::vec3 finalSteering = glm::vec3(0.0f);
//
//	// If we only have one boid on the flock
//	// the steering vector can only be its own direction
//	if (this->_vecBoids.size() == 1)
//	{
//		finalSteering = boidEntity->transform->direction;
//	}
//
//	float flockOtherBoidsSize = this->_vecBoids.size() - 1;
//
//	this->centerPosition = this->flockEntity->transform->position;
//
//	// Calculate cohesion
//	// To calculate the cohesion for a specific boid, we need to make the cohesion vector be
//	// in relation the current boid, by subtracting the position. The divide by the rest amount in relation
//	// to get the average vector in relation to this current boid
//	glm::vec3 cohesion = (this->_cohesion - boidEntity->transform->position + this->centerPosition) / (flockOtherBoidsSize + 1 - boidEntity->transform->position);
//	cohesion = glm::normalize(cohesion);
//	cohesion *= this->_cohesionWeight;
//
//	// Calculate separation
//	// To calculate separation for a specific boid, we need to subtract the position to get the 
//	// amount for all the other boids, then divide by the rest amount to get the average
//	// and reverse the sign since it's a separation
//	glm::vec3 separation = (this->_separation - boidEntity->transform->position) / flockOtherBoidsSize;
//	separation = -glm::normalize(separation);
//	separation *= this->_separationWeight;
//
//	// Calculate alignment
//	// To calculate the alignment for a specific boid, we need to subtract the forward to get
//	// the amount for all the other boids, then divide the rest amount to get the average
//	glm::vec3 alignment = (this->_alignment - boidEntity->transform->direction) / flockOtherBoidsSize;
//	alignment = glm::normalize(alignment);
//	alignment *= this->_alignmentWeight;
//
//	// Calculate final steering by summing all the other vectors
//	// The weights should always all add to 1
//	finalSteering = cohesion + separation + alignment;
//
//	// If the final steering vector is 0, the steering is the boid direction
//	// Should never happen
//	if (glm::length(finalSteering) == 0)
//	{
//		finalSteering = boidEntity->transform->direction;
//	}
//
//	return finalSteering;
//}

glm::vec3 sFlockComponent::GetFlockSteering()
{
	return this->_steering;
}

void sFlockComponent::SetFormation(eFormations newFormation, glm::vec3 newCenterPoint, glm::vec3 newOffset)
{
	this->flockFormation = newFormation;
	if (newFormation != FLOCK)
	{
		this->lastFlockFormation = newFormation;
	}
	this->centerPosition = newCenterPoint;
	this->flockEntity->transform->SetWorldPosition(newCenterPoint);
	this->offset = newOffset;
}

void sFlockComponent::RecallFormation()
{
	this->flockFormation = this->lastFlockFormation;
}

void sFlockComponent::MakeFormation()
{
	this->vecFormationPos.clear();
	switch (this->flockFormation)
	{
	case LINE:
		this->_MakeLineFormation();
		break;
	case V:
		this->_MakeVFormation();
		break;
	case TWOROWS:
		this->_MakeTwoRowsFormation();
		break;
	case SQUARE:
		this->_MakeSquareFormation();
		break;
	case CIRCLE:
		this->_MakeCircleFormation();
		break;
	}
}

void sFlockComponent::AddPathNode(const glm::vec3& newNode)
{
	this->_vecPathNodes.push_back(newNode);
}

void sFlockComponent::SetPathFollow(bool follow, unsigned int mode)
{
	this->followPath = follow;
	this->pathMode = mode;
}

void sFlockComponent::PathFollow(double deltaTime)
{
	if (!this->followPath || this->masterFlock != nullptr || this->_vecPathNodes.size() == 0)
	{
		return;
	}

	float distance = fabs(glm::distance(this->flockEntity->transform->GetWorldPosition(), this->_vecPathNodes[this->currentPathNode]));

	if (distance > 1.0f)
	{
		glm::vec3 direction = this->_vecPathNodes[this->currentPathNode] - this->flockEntity->transform->GetWorldPosition();
		direction = glm::normalize(direction);
		this->flockEntity->transform->GetWorldPosition() += direction * this->flockSpeed * (float)deltaTime;
		this->centerPosition = this->flockEntity->transform->GetWorldPosition();
	}
	else
	{
		if (this->pathMode == 0)
		{
			this->currentPathNode++;
			if (this->currentPathNode >= this->_vecPathNodes.size())
			{
				this->currentPathNode = 0;
			}
		}
		else
		{
			this->currentPathNode--;
			if (this->currentPathNode < 0)
			{
				this->currentPathNode = this->_vecPathNodes.size() - 1;
			}
		}
	}
}

void sFlockComponent::_MakeLineFormation()
{
	this->centerPosition = this->flockEntity->transform->GetWorldPosition();

	// Get amount of alive boids
	int aliveBoids = 0;
	for (unsigned int i = 0; i < this->_vecBoids.size(); i++)
	{
		if (this->_vecBoids[i]->entity->GetIsActive())
		{
			aliveBoids += 1;
		}
	}

	// Find number os positions for each side
	int numEachSide = aliveBoids / 2;

	// Find left most position in relation to center
	glm::vec3 leftMostPoint = centerPosition - ((this->offset) * (float)numEachSide);

	// Make equation to create formation positions based on amount
	// EQ: (leftmostposition) + (offset) * position number
	for (unsigned int i = 0; i < aliveBoids; i++)
	{
		glm::vec3 position = leftMostPoint + (this->offset * (float)i);
		this->vecFormationPos.push_back(position);
	}

	// Set/Move boids into position
	this->MoveFormation();
}

void sFlockComponent::_MakeTwoRowsFormation()
{
	this->centerPosition = this->flockEntity->transform->GetWorldPosition();

	// Get amount of alive boids
	int aliveBoids = 0;
	for (unsigned int i = 0; i < this->_vecBoids.size(); i++)
	{
		if (this->_vecBoids[i]->entity->GetIsActive())
		{
			aliveBoids += 1;
		}
	}

	// Find number os positions for each side
	int numEachRow = aliveBoids / 2;
	int numEachSide = numEachRow / 2;

	// Find left most position in relation to center
	glm::vec3 leftMostPoint = centerPosition - ((this->offset) * (float)numEachSide);

	// Make equation to create formation positions based on amount
	// EQ: (leftmostposition) + (offset) * position number
	int columnNum = 0;
	for (unsigned int i = 0; i < aliveBoids; i++)
	{
		if (i >= numEachRow)
		{
			glm::vec3 position = leftMostPoint + (this->offset * ((float)i - numEachRow));
			position.z -= this->offset.x;
			this->vecFormationPos.push_back(position);
		}
		else
		{
			glm::vec3 position = leftMostPoint + (this->offset * (float)i);
			this->vecFormationPos.push_back(position);
		}
	}

	// Set/Move boids into position
	this->MoveFormation();
}

void sFlockComponent::_MakeVFormation()
{
	this->centerPosition = this->flockEntity->transform->GetWorldPosition();

	// Get amount of alive boids
	int aliveBoids = 0;
	for (unsigned int i = 0; i < this->_vecBoids.size(); i++)
	{
		if (this->_vecBoids[i]->entity->GetIsActive())
		{
			aliveBoids += 1;
		}
	}

	// Find number os positions for each side
	int numEachSide = aliveBoids / 2;

	// Find left most position in relation to center
	glm::vec3 leftMostPoint = centerPosition;
	leftMostPoint.x -= this->offset.x * numEachSide;
	leftMostPoint.z += this->offset.x * numEachSide;

	bool isOdd = (aliveBoids % 2 == 0);

	// Make equation to create formation positions based on amount
	// EQ: (leftmostposition) + (offset) * position number
	for (unsigned int i = 0; i < aliveBoids; i++)
	{
		if (i == 0)
		{
			glm::vec3 position = this->centerPosition;
			this->vecFormationPos.push_back(position);
		}
		else if (i % 2 == 0)
		{
			glm::vec3 position = this->centerPosition;
			position.x += (this->offset.x * (float)i);
			position.z += (this->offset.x * (float)i);
			this->vecFormationPos.push_back(position);
		}
		else
		{
			// 2 * pi / numentities
			glm::vec3 position = this->centerPosition;
			position.x -= (this->offset.x * (float)i);
			position.z += (this->offset.x * (float)i);
			this->vecFormationPos.push_back(position);
		}
	}

	// Set/Move boids into position
	this->MoveFormation();
}

void sFlockComponent::_MakeCircleFormation()
{
	this->centerPosition = this->flockEntity->transform->GetWorldPosition();

	// Get amount of alive boids
	int aliveBoids = 0;
	for (unsigned int i = 0; i < this->_vecBoids.size(); i++)
	{
		if (this->_vecBoids[i]->entity->GetIsActive())
		{
			aliveBoids += 1;
		}
	}

	float dif = 2.0f * glm::pi<float>() / (float)aliveBoids;

	for (unsigned int i = 0; i < aliveBoids; i++)
	{
		float rotation = dif * i;
		float posX = this->centerPosition.x + glm::cos(rotation) * 10.0f;
		float posZ = this->centerPosition.z + glm::sin(rotation) * 10.0f;
		glm::vec3 position = glm::vec3(posX, this->centerPosition.y, posZ);
		this->vecFormationPos.push_back(position);
	}

	// Set/Move boids into position
	this->MoveFormation();
}

void sFlockComponent::_MakeSquareFormation()
{
	this->centerPosition = this->flockEntity->transform->GetWorldPosition();

	// Get amount of alive boids
	int aliveBoids = 0;
	for (unsigned int i = 0; i < this->_vecBoids.size(); i++)
	{
		if (this->_vecBoids[i]->entity->GetIsActive())
		{
			aliveBoids += 1;
		}
	}

	float sqr = glm::inversesqrt<float>(aliveBoids);

	// Find number os positions for each side
	int numEachSide = 1 / sqr;

	// Find left most position in relation to center
	glm::vec3 leftMostPoint = centerPosition;
	leftMostPoint.x -= this->offset.x * (int)(numEachSide/2);
	leftMostPoint.z += this->offset.x * (int)(numEachSide/2);

	int row = 0;
	int column = 0;
	for (unsigned int i = 0; i < aliveBoids; i++)
	{
		glm::vec3 position = leftMostPoint;
		if (i % numEachSide == 0)
		{
			row++;
			column = 0;
		}

		position = leftMostPoint + (this->offset * (float)column);
		float rowDif = (int)(i % numEachSide) * this->offset.x;
		position.z -= this->offset.x * row;
		column++;
		
		this->vecFormationPos.push_back(position);
	}

	this->MoveFormation();
}

void sFlockComponent::MoveFormation()
{
	// Flock: Add center point, add center point to calculation and divide by all plus the center
	// Set/Move boids into position
	int numBoidsSet = 0;
	for (unsigned int i = 0; i < this->_vecBoids.size(); i++)
	{
		sEntity* entity = this->_vecBoids[i]->entity;
		if (!entity->GetIsActive())
		{
			continue;
		}

		float distance = glm::distance(entity->transform->GetWorldPosition(), this->vecFormationPos[numBoidsSet]);
		if (distance < 0.2f)
		{
			entity->transform->GetWorldPosition() = this->vecFormationPos[numBoidsSet];
		}
		else
		{
			glm::vec3 direction = this->vecFormationPos[numBoidsSet] - entity->transform->GetWorldPosition();
			direction = glm::normalize(direction);
			entity->transform->SetWorldPosition(entity->transform->GetWorldPosition() + direction);
		}

		numBoidsSet++;
	}
}

void sFlockComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	if (this->masterFlock == nullptr)
	{
		prettyWriter.Key("FlockIDs"); prettyWriter.StartArray();
		for (unsigned int i = 0; i < this->_vecBoids.size(); i++)
		{
			prettyWriter.Int64(this->_vecBoids[i]->entity->GetInstanceID());
		}
		prettyWriter.EndArray();
	}
	else
	{
		prettyWriter.Key("MasterFlockID"); prettyWriter.Int64(this->masterFlock->flockEntity->GetInstanceID());
	}

	return;
}

void sFlockComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator iter)
{
	if (iter->HasMember("MasterFlockID"))
	{
		this->_masterFlockID = iter->GetInt64();
	}
	if (iter->HasMember("FlockIDs"))
	{
		this->_masterFlockID = iter->GetInt64();
	}

	return;
}

