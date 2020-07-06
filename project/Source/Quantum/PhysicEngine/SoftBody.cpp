#include "stdafx.h"
#include "SoftBody.h"
#include <EngineCore\Logger.h>
#include "PhysicsWorld.h"

namespace physics
{
	SoftBody::SoftBody(float mass, const std::vector<glm::vec3> &vertices, deformation_mode mode, CollisionHull *collision_hull, const ClusterMesh *cluster_mesh)
	{
		type = body_index::SOFTBODY;

		// Stefan: collision hull will be updated once broadphase registers a potential collision
		// cluster_mesh = cluster_mesh->copy(); implementieren!
		collisionHull = collision_hull;

		this->total_mass = mass;
		this->mode = mode;
		initSoftBody(mass, vertices);
		alpha = 1.f;
		beta = 0.4f;
		is_prepared = false;
	}

	SoftBody::~SoftBody()
	{
		for (int i = 0; i < particles.size(); i++)
			delete particles[i];

		delete collisionHull;
	}

	void SoftBody::initSoftBody(float mass, const std::vector<glm::vec3> &vertices)
	{
		bool particlesLoaded = loadParticles(mass, vertices);

		if (!particlesLoaded)
		{
			core::Logger::error("Loading particles failed!");
			return;
		}
		
		precomputeOriginalCOM();
		current_com = original_com;

		switch (this->mode)
		{
		case BASIC:
			precomputeQ();
			initR();
			break;

		case LINEAR:
			precomputeQ();
			precomputeAqqMatrix();
			initALinear();
			break;

		case QUADRATIC:
			precomputeQ();
			precomputeAqqMatrix();
			precomputeQuadQ();
			this->quad_Aqq = Eigen::MatrixXf::Zero(9, 9);
			precomputeQuadAqqMatrix();
			break;

		case CLUSTER:

			break;

		case PLASTIC:

			break;

		default:

			break;
		}

		norm_A = 0.f;
		deviation = 0.f;

		// als Parameter im ctor einfügen
		restitution = 0.9f;
	}

	bool SoftBody::loadParticles(float mass, const std::vector<glm::vec3> &vertices)
	{
		float part_mass = mass / vertices.size();

		particles.reserve(vertices.size());

		float minX = vertices[0].x;
		float minY = vertices[0].y;
		float minZ = vertices[0].z;

		float maxX = vertices[0].x;
		float maxY = vertices[0].y;
		float maxZ = vertices[0].z;

		for (int i = 0; i < vertices.size(); i++)
		{
			// min, max for AABB
			minX = vertices[i].x < minX ? vertices[i].x : minX;
			minY = vertices[i].y < minY ? vertices[i].y : minY;
			minZ = vertices[i].z < minZ ? vertices[i].z : minZ;

			maxX = vertices[i].x > maxX ? vertices[i].x : maxX;
			maxY = vertices[i].y > maxY ? vertices[i].y : maxY;
			maxZ = vertices[i].z > maxZ ? vertices[i].z : maxZ;
			
			Particle* p = new Particle(vertices[i].x, vertices[i].y, vertices[i].z);
			p->mass = part_mass;
		
			particles.push_back(p);
			collisions.push_back(std::vector<SoftBodyParticleCollisionData>());
		}

		glm::vec3 minAABB(minX, minY, minZ);
		glm::vec3 maxAABB(maxX, maxY, maxZ);

		cached_aabb = BroadphaseAABB(minAABB, maxAABB);
		last_change = DynamicsWorld::getCurrentTick();

		if (particles.size() < 1) return false;
		return true;
	}

	void SoftBody::precomputeOriginalCOM()
	{
		original_com = { 0.f, 0.f, 0.f };
		float mass = 0;

		for (int i = 0; i < particles.size(); i++)
		{		
			original_com += particles[i]->init_pos * particles[i]->mass;
			mass += particles[i]->mass;
		}

		original_com /= mass;
	}

	void SoftBody::calcCurrentCOM()
	{
		current_com = glm::vec3(0.f);

		float mass = 0;

		for (int i = 0; i < particles.size(); i++)
		{		
			current_com += particles[i]->intermediate_pos * particles[i]->mass;
			mass += particles[i]->mass;
		}

		current_com /= mass;
	}

	void SoftBody::precomputeQ()
	{
		q.reserve(particles.size());

		for (int i = 0; i < particles.size(); ++i)
		{
			glm::vec3 q_i = particles[i]->init_pos - original_com;
			q.push_back(q_i);
		}
	}

	void SoftBody::precomputeQuadQ()
	{
		quad_q.reserve(particles.size());

		for (int i = 0; i < particles.size(); i++)
		{
			float v[9] = { 0 };

			glm::vec3 q = particles[i]->init_pos - original_com;

			std::array<float, 9> quad_q_i;

			v[0] = q.x;
			v[1] = q.y;
			v[2] = q.z;
			v[3] = q.x * q.x;
			v[4] = q.y * q.y;
			v[5] = q.z * q.z;
			v[6] = q.x * q.y;
			v[7] = q.y * q.z;
			v[8] = q.z * q.x;

			quad_q_i = { v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8] };
			
			quad_q.push_back(quad_q_i);
		}	
	}

	glm::mat3 SoftBody::calcApqMatrix()
	{
		glm::mat3 Apq(0.f);

		for (int i = 0; i < particles.size(); i++)
		{
			glm::vec3 p = particles[i]->intermediate_pos - current_com;

			Apq[0][0] += particles[i]->mass * p.x * q[i].x;
			Apq[1][0] += particles[i]->mass * p.y * q[i].x;
			Apq[2][0] += particles[i]->mass * p.z * q[i].x;

			Apq[0][1] += particles[i]->mass * p.x * q[i].y;
			Apq[1][1] += particles[i]->mass * p.y * q[i].y;
			Apq[2][1] += particles[i]->mass * p.z * q[i].y;

			Apq[0][2] += particles[i]->mass * p.x * q[i].z;
			Apq[1][2] += particles[i]->mass * p.y * q[i].z;
			Apq[2][2] += particles[i]->mass * p.z * q[i].z;
		}

		if (glm::determinant(Apq) < 0.f)
		{
			Apq[0][2] = -Apq[0][2];
			Apq[1][2] = -Apq[1][2];
			Apq[2][2] = -Apq[2][2];
		}
		return glm::transpose(Apq);
	}

	void SoftBody::calcQuadApqMatrix(Eigen::MatrixXf& quad_Apq)
	{
		for (int i = 0; i < particles.size(); i++)
		{
			glm::vec3 btP = particles[i]->intermediate_pos - current_com;
			Eigen::Vector3f p; p << btP.x, btP.y, btP.z;

			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 9; k++)
					quad_Apq(j, k) += particles[i]->mass * p(j) * quad_q[i][k];
		}

		Eigen::Matrix3f firstSquare;

		firstSquare(0, 0) = quad_Apq(0, 0); firstSquare(0, 1) = quad_Apq(0, 1); firstSquare(0, 2) = quad_Apq(0, 2);
		firstSquare(1, 0) = quad_Apq(1, 0); firstSquare(1, 1) = quad_Apq(1, 1); firstSquare(1, 2) = quad_Apq(1, 2);
		firstSquare(2, 0) = quad_Apq(2, 0); firstSquare(2, 1) = quad_Apq(2, 1); firstSquare(2, 2) = quad_Apq(2, 2);

		if (firstSquare.determinant() < 0.f)
		{
			quad_Apq(0, 2) = -quad_Apq(0, 2);
			quad_Apq(1, 2) = -quad_Apq(1, 2);
			quad_Apq(2, 2) = -quad_Apq(2, 2);
		}
	}

	void SoftBody::precomputeAqqMatrix()
	{	
		Aqq = glm::mat3(0.f);
		for (int i = 0; i < particles.size(); i++)
		{
			Aqq[0][0] += particles[i]->mass * q[i].x * q[i].x;
			Aqq[1][0] += particles[i]->mass * q[i].x * q[i].y;
			Aqq[2][0] += particles[i]->mass * q[i].x * q[i].z;

			Aqq[0][1] += particles[i]->mass * q[i].y * q[i].x;
			Aqq[1][1] += particles[i]->mass * q[i].y * q[i].y;
			Aqq[2][1] += particles[i]->mass * q[i].y * q[i].z;

			Aqq[0][2] += particles[i]->mass * q[i].z * q[i].x;
			Aqq[1][2] += particles[i]->mass * q[i].z * q[i].y;
			Aqq[2][2] += particles[i]->mass * q[i].z * q[i].z;
		}
		Aqq = glm::inverse(glm::transpose(Aqq));
	}

	void SoftBody::precomputeQuadAqqMatrix()
	{
		float m_9x9[9][9] = { 0 };

		for (int i = 0; i < particles.size(); i++)
		{		
			for (int j = 0; j < 9; j++)
				for (int k = 0; k < 9; k++)
					m_9x9[j][k] += particles[i]->mass * quad_q[i][j] * quad_q[i][k];			
		}
		
		quad_Aqq << m_9x9[0][0], m_9x9[0][1], m_9x9[0][2], m_9x9[0][3], m_9x9[0][4], m_9x9[0][5], m_9x9[0][6], m_9x9[0][7], m_9x9[0][8],
					m_9x9[1][0], m_9x9[1][1], m_9x9[1][2], m_9x9[1][3], m_9x9[1][4], m_9x9[1][5], m_9x9[1][6], m_9x9[1][7], m_9x9[1][8],
					m_9x9[2][0], m_9x9[2][1], m_9x9[2][2], m_9x9[2][3], m_9x9[2][4], m_9x9[2][5], m_9x9[2][6], m_9x9[2][7], m_9x9[2][8],
					m_9x9[3][0], m_9x9[3][1], m_9x9[3][2], m_9x9[3][3], m_9x9[3][4], m_9x9[3][5], m_9x9[3][6], m_9x9[3][7], m_9x9[3][8],
					m_9x9[4][0], m_9x9[4][1], m_9x9[4][2], m_9x9[4][3], m_9x9[4][4], m_9x9[4][5], m_9x9[4][6], m_9x9[4][7], m_9x9[4][8],
					m_9x9[5][0], m_9x9[5][1], m_9x9[5][2], m_9x9[5][3], m_9x9[5][4], m_9x9[5][5], m_9x9[5][6], m_9x9[5][7], m_9x9[5][8],
					m_9x9[6][0], m_9x9[6][1], m_9x9[6][2], m_9x9[6][3], m_9x9[6][4], m_9x9[6][5], m_9x9[6][6], m_9x9[6][7], m_9x9[6][8],
					m_9x9[7][0], m_9x9[7][1], m_9x9[7][2], m_9x9[7][3], m_9x9[7][4], m_9x9[7][5], m_9x9[7][6], m_9x9[7][7], m_9x9[7][8],
					m_9x9[8][0], m_9x9[8][1], m_9x9[8][2], m_9x9[8][3], m_9x9[8][4], m_9x9[8][5], m_9x9[8][6], m_9x9[8][7], m_9x9[8][8];

		quad_Aqq = quad_Aqq.inverse();
	}

	glm::mat3 SoftBody::calcRotationMatrix(const glm::mat3& matrix)
	{
		Eigen::Matrix3f m;

		m(0, 0) = matrix[0][0];
		m(0, 1) = matrix[1][0];
		m(0, 2) = matrix[2][0];

		m(1, 0) = matrix[0][1];
		m(1, 1) = matrix[1][1];
		m(1, 2) = matrix[2][1];

		m(2, 0) = matrix[0][2];
		m(2, 1) = matrix[1][2];
		m(2, 2) = matrix[2][2];
		m.transpose();

		Eigen::JacobiSVD<Eigen::Matrix3f> svd(m, Eigen::ComputeFullU | Eigen::ComputeFullV);

		Eigen::Matrix3f eU = svd.matrixU();
		Eigen::Matrix3f eV = svd.matrixV();
						
		Eigen::Matrix3f rot = eV * eU.transpose();

		float detRot = rot.determinant();
	
		if (detRot < 0.f)
		{			
			eV(2, 0) = -eV(2, 0);
			eV(2, 1) = -eV(2, 1);
			eV(2, 2) = -eV(2, 2);

			rot = eV * eU.transpose();
		}
		
		glm::mat3 glm_rot(0.f);

		glm_rot[0][0] = rot(0, 0);
		glm_rot[1][0] = rot(0, 1);
		glm_rot[2][0] = rot(0, 2);

		glm_rot[0][1] = rot(1, 0);
		glm_rot[1][1] = rot(1, 1);
		glm_rot[2][1] = rot(1, 2);

		glm_rot[0][2] = rot(2, 0);
		glm_rot[1][2] = rot(2, 1);
		glm_rot[2][2] = rot(2, 2);

		glm_rot = glm::transpose(glm_rot);

		return glm_rot;
	}

	BroadphaseAABB SoftBody::recalculateAABB()
	{
		return cached_aabb;
	}

	void SoftBody::initR()
	{
		glm::mat3 Apq = calcApqMatrix();
		R = calcRotationMatrix(Apq);
	}

	void SoftBody::initALinear()
	{
		glm::mat3 Apq = calcApqMatrix();
		A = Apq * Aqq;
	}
}