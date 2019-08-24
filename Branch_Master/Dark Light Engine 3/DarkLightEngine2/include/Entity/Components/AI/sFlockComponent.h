#ifndef _cFlockComponent_HG_
#define _sFlockComponent_HG_

#include <Entity\Components\iComponent.h>
#include <OpenGL\OpenGLHeaders.h>
#include <vector>

struct sEntity;


struct sFlockComponent : public iComponent
{
public:
	sFlockComponent(unsigned long long entityID);
	~sFlockComponent();

	enum eFormations
	{
		FLOCK,
		LINE,
		TWOROWS,
		V,
		SQUARE,
		CIRCLE,
		DIAMOND,
	};

	struct sBoid
	{
		sBoid(sEntity* entity)
		{
			this->entity = entity;
		}

		sEntity* entity;
	};

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "FLOCK_COMPONENT"; };

	void SetWeights(float cohesionW, float separationW, float alignmentW);

	bool AddBoid(sEntity* entity);
	bool RemoveBoid(sEntity* entity);

	void UpdateBoidsInRange();

	glm::vec3 CalculateCohesionFor(sEntity* entity);
	glm::vec3 CalculateSeparationFor(sEntity* entity);
	glm::vec3 CalculateAlignmentFor(sEntity* entity);

	void CalculateVectors();
	void CalculateSteering();

	glm::vec3 GetSteeringVectorFor(sEntity* entity);
	glm::vec3 GetFlockSteering();

	void SetFormation(eFormations newFormation, glm::vec3 newCenterPoint, glm::vec3 newOffset);
	void RecallFormation();
	void MakeFormation();
	void MoveFormation();

	void SetPathFollow(bool follow, unsigned int mode);
	void PathFollow(double deltaTime);
	void AddPathNode(const glm::vec3& newNode);

	unsigned int flockTotalSize;
	unsigned int flockCurrentSize;
	float flockRadius;
	float flockSpeed;

	eFormations flockFormation;
	eFormations lastFlockFormation;
	glm::vec3 centerPosition;
	glm::vec3 offset;
	std::vector<glm::vec3> vecFormationPos;

	sFlockComponent* masterFlock;
	sEntity* flockEntity;
	std::vector<sBoid*> _vecBoids;

	bool followPath;
	unsigned int currentPathNode;
	unsigned int pathMode;
	
	std::vector<glm::vec3> _vecPathNodes;

	// The values of each vector. They should sum all to 1
	float _cohesionWeight;
	float _separationWeight;
	float _alignmentWeight;

protected:
	void _MakeLineFormation();
	void _MakeVFormation();
	void _MakeTwoRowsFormation();
	void _MakeSquareFormation();
	void _MakeCircleFormation();

	void _AssembleFlock();

	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

	// The vectors that form the steering direction
	glm::vec3 _cohesion;
	glm::vec3 _separation;
	glm::vec3 _alignment;
	glm::vec3 _steering;

	unsigned long long _masterFlockID;
	std::vector<unsigned long long> _vecFlockIDs;

};



#endif // !_cFlockComponent_HG_

