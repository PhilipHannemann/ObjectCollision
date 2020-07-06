#pragma once
#include "AlgorithmBase.h"
#include "SphereTreeNode.h"
#include "SoftBody.h"
#include <unordered_map>

namespace physics
{
	class CollisionWorld;

	class SBSphereTreeCollisionAlgorithm : public ACollisionAlgorithm
	{
	public:	
		SBSphereTreeCollisionAlgorithm();
		virtual ~SBSphereTreeCollisionAlgorithm();

		void testPair(CollisionBody*& bodyA, CollisionBody*& bodyB, const glm::vec3 &gravity, const float &air_drag, const float &time_delta);

	private:

		void prepareTraversal(SoftBody* body);

		void updateNode(SoftBody* body, SphereTreeNode* node);

		float calcANormLinear(SoftBody* body);

		float calcDeviationBasic(SoftBody*& body);

		float calcDeviationLinear(SoftBody* body);

		void testNodes(SoftBody* bodyA, SoftBody* bodyB, SphereTreeNode* nodeA, SphereTreeNode* nodeB, const glm::vec3 &gravity, const float &air_drag, std::unordered_map<SphereTreeNode*, std::vector<SphereTreeNode*>> &pcollisionsA, std::unordered_map<SphereTreeNode*, std::vector<SphereTreeNode*>> &pcollisionsB);

		bool sphereSphereIntersects(SphereTreeNode* nodeA, SphereTreeNode* nodeB);

		bool pointSphereIntersects(const glm::vec3 &pos, const SphereTreeNode* node);

		void sphereSphereCollision(SoftBody* bodyA, SoftBody* bodyB, std::unordered_map<SphereTreeNode*, std::vector<SphereTreeNode*>> &pcollisions, const glm::vec3 &gravity, const float &air_drag);

		void checkParticles(SoftBody* bodyA, const SphereTreeNode* nodeA, SoftBody* bodyB, const SphereTreeNode* nodeB, const glm::vec3 &gravity, const float &air_drag);

		glm::vec3 calcSurfaceNormal(const SoftBody* body, const SphereTreeNode* node);

		glm::vec3 projectPointOntoPlane(const glm::vec3 &point, const glm::vec3 &plane_normal, const glm::vec3 &plane_point);

		bool segmentPlaneIntersects(
			const glm::vec3 &segment_start, 
			const glm::vec3 &segment_end, 
			const glm::vec3 &plane_normal, 
			const glm::vec3 &plane_point, 
			glm::vec3 &contact_point);

		bool barycentric(glm::vec3 &p, glm::vec3 &a, glm::vec3 &b, glm::vec3 &c, float &u, float &v, float &w);

		void calcReflectedVelocity(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &normal, const float &restitution, const float &m1, const float &m2, glm::vec3 &res_v1, glm::vec3 &res_v2);
	};
}