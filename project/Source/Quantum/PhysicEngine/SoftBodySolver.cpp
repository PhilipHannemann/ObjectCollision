#include "stdafx.h"
#include "SoftBodySolver.h"
#include "PhysicsWorld.h"
#include <EngineCore\Logger.h>

namespace physics
{
	SoftBodySolver::SoftBodySolver(){}

	SoftBodySolver::~SoftBodySolver(){}

	void SoftBodySolver::processCollision(SoftBody*& body, const glm::vec3 &gravity, const float &time_delta)
	{
		reflectVelocity(body, time_delta);

		switch (body->mode)
		{
		case BASIC:
			calcGoalPosBasic(body);
			break;
		case LINEAR:
			calcGoalPosLinear(body);
			break;
		case QUADRATIC:
			calcGoalPosQuadratic(body);
			break;
		case CLUSTER:
			calcGoalPosCluster(body);
			break;
		case PLASTIC:
			calcGoalPosPlastic(body);
			break;
		default:
			calcGoalPosBasic(body);
			break;
		}

		integratePosition(body, time_delta);
	}

	void SoftBodySolver::intermediatePositions(SoftBody*& body, const glm::vec3 &gravity, const float &time_delta)
	{
		bool first_iteration = true;

		float minX = 0.f;
		float minY = 0.f;
		float minZ = 0.f;

		float maxX = 0.f;
		float maxY = 0.f;
		float maxZ = 0.f;

		for (int i = 0; i < body->particles.size(); i++)
		{	
			body->particles[i]->applied_force += (gravity * body->particles[i]->mass);

			body->particles[i]->velocity += (body->particles[i]->applied_force / body->particles[i]->mass) * time_delta;
			body->particles[i]->intermediate_pos = body->particles[i]->pos + body->particles[i]->velocity * time_delta;
			body->particles[i]->goal_pos = body->particles[i]->init_pos;
		
			if (first_iteration)
			{
				minX = body->particles[0]->intermediate_pos.x;
				minY = body->particles[0]->intermediate_pos.y;
				minZ = body->particles[0]->intermediate_pos.z;

				maxX = body->particles[0]->intermediate_pos.x;
				maxY = body->particles[0]->intermediate_pos.y;
				maxZ = body->particles[0]->intermediate_pos.z;

				first_iteration = false;
			}

			// min, max for AABB
			minX = body->particles[i]->intermediate_pos.x < minX ? body->particles[i]->intermediate_pos.x : minX;
			minY = body->particles[i]->intermediate_pos.y < minY ? body->particles[i]->intermediate_pos.y : minY;
			minZ = body->particles[i]->intermediate_pos.z < minZ ? body->particles[i]->intermediate_pos.z : minZ;

			maxX = body->particles[i]->intermediate_pos.x > maxX ? body->particles[i]->intermediate_pos.x : maxX;
			maxY = body->particles[i]->intermediate_pos.y > maxY ? body->particles[i]->intermediate_pos.y : maxY;
			maxZ = body->particles[i]->intermediate_pos.z > maxZ ? body->particles[i]->intermediate_pos.z : maxZ;
		}

		glm::vec3 minAABB(minX, minY, minZ);
		glm::vec3 maxAABB(maxX, maxY, maxZ);

		body->cached_aabb.recalculateAABB(minAABB, maxAABB);
		body->last_change = DynamicsWorld::getCurrentTick();

		first_iteration = true;
	}	

	void SoftBodySolver::calcGoalPosBasic(SoftBody*& body)
	{
		body->calcCurrentCOM();
		
		glm::mat3 Apq = body->calcApqMatrix();
		glm::mat3 R = body->calcRotationMatrix(Apq);

		for (int i = 0; i < body->particles.size(); i++)
		{
			glm::vec3 goal = R * body->q[i] + body->current_com;
			body->particles[i]->goal_pos = goal;
		}
	}

	void SoftBodySolver::calcGoalPosLinear(SoftBody*& body)
	{
		body->calcCurrentCOM();

		glm::mat3 Apq = body->calcApqMatrix();
		body->A = Apq * body->Aqq;

		// conserve volume
		float detA = glm::determinant(body->A);
		float cbrt = pow(fabs(detA), 1.f / 3.f);
		cbrt = (detA < 0.f) ? -cbrt : cbrt;
		body->A = body->A * (1.f / cbrt);

		// calculate rotation matrix
		glm::mat3 R = body->calcRotationMatrix(/*body->A*/Apq);

		// component beta * A + (1 - beta) * R of linear deformation
		glm::mat3 A_local = body->beta * body->A;
		R = (1.f - body->beta) * R;
		glm::mat3 T = A_local + R;

		for (int i = 0; i < body->particles.size(); i++)
		{
			glm::vec3 goal = (T * body->q[i]) + body->current_com;
			body->particles[i]->goal_pos = goal;
			//body->particles[i]->intermediate_pos += (body->particles[i]->goal_pos - body->particles[i]->intermediate_pos) * body->alpha;
		}
	}

	void SoftBodySolver::calcGoalPosQuadratic(SoftBody*& body)
	{
		body->calcCurrentCOM();

		Eigen::MatrixXf quad_Apq = Eigen::MatrixXf::Zero(3, 9);
		body->calcQuadApqMatrix(quad_Apq);

		Eigen::MatrixXf quad_A = quad_Apq * body->quad_Aqq;
		
		Eigen::Matrix3f sub_quad_A;

		sub_quad_A(0, 0) = quad_A(0, 0); sub_quad_A(0, 1) = quad_A(0, 1); sub_quad_A(0, 2) = quad_A(0, 2);
		sub_quad_A(1, 0) = quad_A(1, 0); sub_quad_A(1, 1) = quad_A(1, 1); sub_quad_A(1, 2) = quad_A(1, 2);
		sub_quad_A(2, 0) = quad_A(2, 0); sub_quad_A(2, 1) = quad_A(2, 1); sub_quad_A(2, 2) = quad_A(2, 2);
		
		float detSubA = sub_quad_A.determinant();
		
		if (detSubA != 0.0f) {
			detSubA = 1.0f / sqrt(fabs(detSubA));
			if (detSubA > 2.0f) detSubA = 2.0f;
			quad_A(0, 0) *= detSubA; quad_A(0, 1) *= detSubA; quad_A(0, 2) *= detSubA;
			quad_A(1, 0) *= detSubA; quad_A(1, 1) *= detSubA; quad_A(1, 2) *= detSubA;
			quad_A(2, 0) *= detSubA; quad_A(2, 1) *= detSubA; quad_A(2, 2) *= detSubA;
		}

		glm::mat3 Apq = body->calcApqMatrix();
		glm::mat3 A = Apq * body->Aqq;
		glm::mat3 R = body->calcRotationMatrix(A);
		
		Eigen::MatrixXf quad_R = Eigen::MatrixXf::Zero(3, 9);

		quad_R(0, 0) = R[0][0]; quad_R(0, 1) = R[1][0]; quad_R(0, 2) = R[2][0];
		quad_R(1, 0) = R[0][1]; quad_R(1, 1) = R[1][1]; quad_R(1, 2) = R[2][1];
		quad_R(2, 0) = R[0][2]; quad_R(2, 1) = R[1][2]; quad_R(2, 2) = R[2][2];

		Eigen::MatrixXf quad_T = Eigen::MatrixXf::Zero(3, 9);

		quad_A = body->beta * quad_A;
		quad_R = (1 - body->beta) * quad_R;		

		quad_T = quad_A + quad_R;

		Eigen::VectorXf q = Eigen::VectorXf::Zero(9);

		for (int i = 0; i < body->particles.size(); i++)
		{

			q << body->quad_q[i][0],
				body->quad_q[i][1],
				body->quad_q[i][2],
				body->quad_q[i][3],
				body->quad_q[i][4],
				body->quad_q[i][5],
				body->quad_q[i][6],
				body->quad_q[i][7],
				body->quad_q[i][8];

			Eigen::Vector3f goal = quad_T * q;

			glm::vec3 glmGoal = { (float)goal(0), (float)goal(1), (float)goal(2) };
			glmGoal += body->current_com;

			body->particles[i]->goal_pos = glmGoal;
		}
	}

	void SoftBodySolver::calcGoalPosCluster(SoftBody*& body)
	{
		body->calcCurrentCOM();
	}

	void SoftBodySolver::calcGoalPosPlastic(SoftBody*& body)
	{
		body->calcCurrentCOM();
	}

	void SoftBodySolver::reflectVelocity(SoftBody*& body, const float &time_delta)
	{
		//int ii = 0;
		for (int i = 0; i < body->particles.size(); i++)
		{
			Particle *p = body->particles[i];
			std::vector<SoftBodyParticleCollisionData> &col = body->collisions[i];

			if (col.size() > 0)
			{
				p->velocity = glm::vec3(0);
			}


			for(int j = 0; j < col.size(); j++)
			{
				SoftBodyParticleCollisionData &cd = col[j];
		
				p->intermediate_pos = cd.contact_point;
				p->velocity += cd.reflected_velocity;				
			}	

			if (col.size() > 0)
			{
				p->intermediate_pos += p->velocity * time_delta;
			}
			
		}
		//core::Logger::info("COLS: " + std::to_string(ii));
	}

	void SoftBodySolver::integratePosition(SoftBody*& body, const float &time_delta)
	{
		for (int i = 0; i < body->particles.size(); i++)
		{
			//glm::vec3 ip = body->particles[i]->intermediate_pos + body->particles[i]->velocity * time_delta;
			glm::vec3 vv = (body->alpha * (body->particles[i]->goal_pos - body->particles[i]->intermediate_pos) / time_delta)
				+ (body->particles[i]->applied_force /  body->particles[i]->mass * time_delta);
			body->particles[i]->velocity += vv;
			
			body->particles[i]->pos = body->particles[i]->intermediate_pos + vv * time_delta;

			/*body->particles[i]->velocity = (body->particles[i]->intermediate_pos - body->particles[i]->pos) * (1.f / time_delta);
			body->particles[i]->pos = body->particles[i]->intermediate_pos;*/

			body->particles[i]->narrowphase_test_state = Particle::NarrowPhaseTestState::UNTESTED;
			//if (body->collisions.find(i) != body->collisions.end()) body->collisions.erase(i);	

			body->particles[i]->applied_force = glm::vec3(0);

			body->collisions[i].clear();

			body->is_prepared = false;
		}
	}

	void SoftBodySolver::fill3x9(Eigen::MatrixXf &target, Eigen::Matrix3f &m1, Eigen::Matrix3f &m2, Eigen::Matrix3f &m3)
	{
		target(0, 0) = m1(0, 0); target(0, 1) = m1(0, 1); target(0, 2) = m1(0, 2);
		target(1, 0) = m1(1, 0); target(1, 1) = m1(1, 1); target(1, 2) = m1(1, 2);
		target(2, 0) = m1(2, 0); target(2, 1) = m1(2, 1); target(2, 2) = m1(2, 2);

		target(0, 3) = m2(0, 0); target(0, 4) = m2(0, 1); target(0, 5) = m2(0, 2);
		target(1, 3) = m2(1, 0); target(1, 4) = m2(1, 1); target(1, 5) = m2(1, 2);
		target(2, 3) = m2(2, 0); target(2, 4) = m2(2, 1); target(2, 5) = m2(2, 2);

		target(0, 6) = m3(0, 0); target(0, 7) = m3(0, 1); target(0, 8) = m3(0, 2);
		target(1, 6) = m3(1, 0); target(1, 7) = m3(1, 1); target(1, 8) = m3(1, 2);
		target(2, 6) = m3(2, 0); target(2, 7) = m3(2, 1); target(2, 8) = m3(2, 2);
	}

	void SoftBodySolver::extractFirst3x3(Eigen::MatrixXf &target, Eigen::Matrix3f &result)
	{
		result(0, 0) = target(0, 0); result(0, 1) = target(0, 1); result(0, 2) = target(0, 2);
		result(1, 0) = target(1, 0); result(1, 1) = target(1, 1); result(1, 2) = target(1, 2);
		result(2, 0) = target(2, 0); result(2, 1) = target(2, 1); result(2, 2) = target(2, 2);
	}
}