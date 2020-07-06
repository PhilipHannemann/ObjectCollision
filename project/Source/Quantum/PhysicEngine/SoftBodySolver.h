#pragma once

#include "SoftBody.h"
#include "SphereTreeCollisionHull.h"
#include "CollisionData.h"
#include "BodyIndex.h"
#include <unordered_map>
#include <glm\glm.hpp>

namespace physics
{
	class SoftBodySolver
	{		
	private:
		// basic deformation
		void calcGoalPosBasic(SoftBody*& body);

		// linear deformation
		void calcGoalPosLinear(SoftBody*& body);

		// quadratic deformation
		void calcGoalPosQuadratic(SoftBody*& body);

		// cluster based deformation
		void calcGoalPosCluster(SoftBody*& body);

		// plastic deformation
		void calcGoalPosPlastic(SoftBody*& body);

		void reflectVelocity(SoftBody*& body, const float &time_delta);

		// integration (moving particles)
		void integratePosition(SoftBody*& body, const float &time_delta);

		// Helper: fill 3x9-matrix with 3 3x3-matrices
		void fill3x9(Eigen::MatrixXf &target, Eigen::Matrix3f &m1, Eigen::Matrix3f &m2, Eigen::Matrix3f &m3);

		// Helper: extract first 3x3-matrix from a 3x9-matrix
		void extractFirst3x3(Eigen::MatrixXf &target, Eigen::Matrix3f &result);

	public:
		SoftBodySolver();
		~SoftBodySolver();

		// symplectic euler first step
		void intermediatePositions(SoftBody*& body, const glm::vec3 &gravity, const float &time_delta);

		void processCollision(
			SoftBody*& body,
			const glm::vec3 &gravity,
			const float &time_delta
		);
	};
}