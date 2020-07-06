#pragma once

#include <Eigen/Dense>
#include "Particle.h"
#include "DeformationMode.h"
#include <vector>
#include <array>
#include <unordered_map>
#include <glm\glm.hpp>
#include "CollisionBody.h"
#include "UserData.h"
#include "SphereTreeCollisionHull.h"
#include "ClusterMesh.h"

//#define RENDER_PARTICLE_POS_SMOOTH

namespace physics
{
	class SoftBody : public CollisionBody
	{
	public:
		SoftBodyUserData userdata;
		
		// stiffness
		float alpha;

		// goal bosition deformation
		float beta;

		// total mass for reference
		float total_mass;

		// deformation mode
		deformation_mode mode;

		// center of mass
		glm::vec3 original_com;
		glm::vec3 current_com;

		// relative locations q_i 
		// (precomputed during initialization of the soft body)
		std::vector<glm::vec3> q;
		std::vector<std::array<float, 9>> quad_q;

		// particles
		std::vector<Particle*> particles;

		// matrices
		// (Aqq)^-1 [3x3] (precomputed during initialization of the soft body)
		glm::mat3 Aqq;
		// quad_Aqq [9x9] (precomputed during initialization of the soft body)
		Eigen::MatrixXf quad_Aqq;

		// basic transformation matrix
		glm::mat3 R;
		// linear transformation matrix
		glm::mat3 A;

		// used in collision detection and response
		float norm_A;
		float deviation;
		float restitution;
		std::vector<std::vector<SoftBodyParticleCollisionData>> collisions;
		//std::vector<SoftBodyParticleCollisionData> collisions;

		//
		// Constructor / Destructor
		//
		SoftBody(float mass, 
			const std::vector<glm::vec3> &vertices, 
			deformation_mode mode, 
			CollisionHull *collision_hull, 
			const ClusterMesh *cluster_mesh);

		 ~SoftBody();

		//
		// Methods
		//
		bool loadParticles(float mass, const std::vector<glm::vec3> &vertices);
		void calcCurrentCOM();
		glm::mat3 calcApqMatrix();
		void calcQuadApqMatrix(Eigen::MatrixXf& quad_Apq);
		glm::mat3 calcRotationMatrix(const glm::mat3& matrix);		

		virtual BroadphaseAABB recalculateAABB() override;
	
		BroadphaseAABB cached_aabb;

		bool is_prepared;

	private:
		//
		// private methods
		//
		void initSoftBody(float mass, const std::vector<glm::vec3> &vertices);
		void precomputeOriginalCOM();
		void precomputeQ();
		void precomputeQuadQ();
		void precomputeAqqMatrix();
		void precomputeQuadAqqMatrix();
		void initR();
		void initALinear();
	};
}