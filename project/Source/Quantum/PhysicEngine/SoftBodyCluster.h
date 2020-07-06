#pragma once
#include "DeformationMode.h"
#include "Particle.h"

namespace physics
{
	class SoftBodyCluster
	{
	public:
		//// TODO: explain a variable with meaning, but no official explanation
		//float alpha;

		//// TODO: explain a variable with meaning, but no official explanation
		//float beta;

		//// deformation mode
		//deformation_mode mode;

		//// center of mass
		//glm::vec3 original_com;
		//glm::vec3 current_com;

		//// relative locations q_i 
		//// (precomputed during initialization of the soft body)
		//std::vector<glm::vec3> q;
		//std::vector<std::array<float, 9>> quad_q;

		//// particles
		//std::vector<Particle*> particles;

		//// matrices
		//// (Aqq)^-1 [3x3] (precomputed during initialization of the soft body)
		//glm::mat3 Aqq;
		//// quad_Aqq [9x9] (precomputed during initialization of the soft body)
		//Eigen::MatrixXf quad_Aqq;

		//// linear transformation matrix
		//glm::mat3 A;

		//SoftBodyCluster();
		//~SoftBodyCluster();
	};
}