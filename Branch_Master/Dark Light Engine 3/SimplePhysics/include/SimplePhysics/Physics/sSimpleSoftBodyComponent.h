#ifndef _sSimpleSoftBodyComponent_HG_
#define _sSimpleSoftBodyComponent_HG_

#include <Interfaces\Physics\iSoftbody.h>
#include <Interfaces\Physics\sSoftbodyDef.h>
#include <Interfaces\Physics\iSphereShape.h>
#include <vector>


namespace nPhysics
{
	class cSimplePhysicsSystem;
	struct sSimpleSoftBodyComponent : public iSoftbody
	{
		friend class cSimplePhysicsSystem;
		class cNode;
		class cSpring
		{
		public:
			cSpring(cNode* newNodeA, cNode* newNodeB, float newSpringconstant);

			void ApplyForce();
			cNode* GetOtherNode(cNode* node);

			cNode* nodeA;
			cNode* nodeB;
			float restingDistance;
			float springConstant;
		};

		class cNode
		{
		public:
			cNode(const glm::vec3& newPos, const glm::vec3& newRelativePos, float newMass);
			~cNode();

			inline bool IsFixed() { return this->mass == 0.0f; };
			bool HasNeighbour(cNode* node);

			void ComputeRadius();

			void Integrate(double deltaTime);

			iSphereShape* sphere;
			glm::vec3 position;
			glm::vec3 localPosition;
			glm::vec3 lastPosition;
			glm::vec3 parentPosition;
			glm::vec3 velocity;
			glm::vec3 springForce;
			float radius;
			float mass;
			std::vector<cSpring*> vecSprings;
		};

	public:
		sSimpleSoftBodyComponent(const sSoftbodyDef& def, unsigned long long entityID);
		~sSimpleSoftBodyComponent();

		virtual void Start();

		// This must be overriden on the derived components, otherwise it will return an error!
		static std::string GetStaticType() { return "SOFTBODY_COMPONENT"; };

		// Set transform
		virtual glm::mat4 GetTransform();
		virtual void SetTransform(const glm::mat4& transform);

		// Manage main entity transform component
		virtual sLocationComponent* GetTransformComponent();
		virtual void SetTransformComponent(sLocationComponent* locationC);

		virtual eCollisionLayerMask GetCollisionLayerMask();
		virtual void SetCollisionLayerMask(eCollisionLayerMask layerMask);

		void _GenerateSoftbody();

		// Returns the total number of nodes
		virtual unsigned int GetNumNodes();

		// Return the node position by index
		virtual glm::vec3 GetNodePosition(unsigned int index);
		virtual float GetNodeRadius(unsigned int index);

		// Get an AABB based on position calculation
		virtual void GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut);

		void UpdateInternal(double deltaTime, const glm::vec3& gravity);

		void Cleanup();

		float springConstant;
		unsigned int numNodesAB;
		unsigned int numNodesAC;
		glm::vec3 cornerA;
		glm::vec3 cornerB;
		glm::vec3 cornerC;

	protected:
		sSimpleSoftBodyComponent(const sSimpleSoftBodyComponent& other) : iSoftbody(other) {};
		sSimpleSoftBodyComponent& operator=(const sSimpleSoftBodyComponent& other) { return *this; };

		virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
		virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

		void _CollideNodes(cNode* nodeA, cNode* nodeB, double deltaTime);
		void _UpdateAABB();

		sLocationComponent* _transform;

		// Bounds
		glm::vec3 _minBounds;
		glm::vec3 _maxBounds;

		// Wind
		glm::vec3 _windDirection;
		float _windMagnitude;
		float _windForce;
		float _windYaw;
		float _windPitch;

		std::vector<cNode*> _vecNodes;
		std::vector<cSpring*> _vecSprings;

	};
}


#endif // !_sSimpleSoftBodyComponent_HG_

