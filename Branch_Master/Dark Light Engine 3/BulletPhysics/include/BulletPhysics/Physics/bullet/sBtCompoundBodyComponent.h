#ifndef _sBtCompoundBodyComponent_HG_
#define _sBtCompoundBodyComponent_HG_

#include "BulletConstraints\iBulletConstraint.h"
#include "iBulletCompoundBody.h"
#include <Interfaces\Physics\sCompoundbodyDef.h>
#include <Interfaces\Physics\iCompoundBody.h>
#include <BulletDynamics\ConstraintSolver\btTypedConstraint.h>
#include <vector>

struct sLocationComponent;


namespace nPhysics
{
	struct sBtRigidbodyComponent;
	struct sBtCompoundBodyComponent : public iCompoundBody
	{
	public:
		sBtCompoundBodyComponent(const sCompoundbodyDef& def, unsigned long long entityID);
		virtual ~sBtCompoundBodyComponent();

		// Start method for the component
		virtual void Start();

		// This must be overriden on the derived components, otherwise it will return an error!
		static std::string GetStaticType() { return "COMPOUNDBODY_COMPONENT"; };

		virtual void AddCompoundBody(iRigidbody* body, bool addID = true);

		virtual void AddConstraint(eConstraintTypes constraintType, unsigned int indexA, const glm::vec3& pivotInA, const glm::vec3& axisInA);
		virtual void AddConstraint(eConstraintTypes constraintType, unsigned int indexA, unsigned int indexB, const glm::vec3& pivotInA, const glm::vec3& pivotInB);
		virtual void AddConstraint(eConstraintTypes constraintType, unsigned int indexA, unsigned int indexB, const glm::vec3& pivotInA, const glm::vec3& pivotInB,
			const glm::vec3& axisInA, const glm::vec3& axisInB);

		unsigned int GetNumRigidbodies();
		unsigned int GetNumConstraints();

		// Set transform
		virtual glm::mat4 GetTransform();
		virtual void SetTransform(const glm::mat4& transform);

		// Manage main entity transform component
		virtual sLocationComponent* GetTransformComponent();
		virtual void SetTransformComponent(sLocationComponent* locationC);

		virtual eCollisionLayerMask GetCollisionLayerMask();
		virtual void SetCollisionLayerMask(eCollisionLayerMask layerMask);

		iRigidbody* GetRigidbody(unsigned int index);
		btTypedConstraint* GetBtConstraint(unsigned int index);

		// Get an AABB based on position calculation
		virtual void GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut);

		sLocationComponent* transform;

	protected:
		virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
		virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

		std::vector<sBtRigidbodyComponent*> _vecRigidbodies;
		std::vector<iBulletConstraint*> _vecConstraints;

	};
}



#endif // !_sBulletCompoundBody_HG_

