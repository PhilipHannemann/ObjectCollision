#include "stdafx.h"
#include "SBSphereTreeCollisionAlgorithm.h"
#include <EngineCore\Logger.h>
#include <glm\gtx\normal.hpp>
#include "PhysicsWorld.h"

namespace physics
{
	SBSphereTreeCollisionAlgorithm::SBSphereTreeCollisionAlgorithm()
	{

	}

	SBSphereTreeCollisionAlgorithm::~SBSphereTreeCollisionAlgorithm()
	{
		
	}

	void SBSphereTreeCollisionAlgorithm::testPair(CollisionBody*& bodyA, CollisionBody*& bodyB, const glm::vec3 &gravity, const float &air_drag, const float &time_delta)
	{
		SoftBody* sbA = static_cast<SoftBody*>(bodyA);
		SoftBody* sbB = static_cast<SoftBody*>(bodyB);
		SphereTreeCollisionHull* hullA = static_cast<SphereTreeCollisionHull*>(bodyA->collisionHull);
		SphereTreeCollisionHull* hullB = static_cast<SphereTreeCollisionHull*>(bodyB->collisionHull);

		prepareTraversal(sbA);
		prepareTraversal(sbB);
		

		std::unordered_map<SphereTreeNode*, std::vector<SphereTreeNode*>> pcollisionsA;
		std::unordered_map<SphereTreeNode*, std::vector<SphereTreeNode*>> pcollisionsB;

		testNodes(sbA, sbB, hullA->root, hullB->root, gravity, air_drag, pcollisionsA, pcollisionsB);

		// TODO: process pcollisions
		sphereSphereCollision(sbA, sbB, pcollisionsA, gravity, air_drag);
		sphereSphereCollision(sbB, sbA, pcollisionsB, gravity, air_drag);

	}

	void SBSphereTreeCollisionAlgorithm::prepareTraversal(SoftBody* body)
	{
		if (!body->is_prepared)
		{
			switch (body->mode)
			{
			case BASIC:
				body->norm_A = 1.f;
				body->deviation = calcDeviationBasic(body);
				break;

			case LINEAR:
				body->norm_A = calcANormLinear(body);
				body->deviation = calcDeviationLinear(body);
				break;

			case QUADRATIC:

				break;

			case CLUSTER:

				break;

			case PLASTIC:

				break;
			}
		}
		//core::Logger::error("NA: " + std::to_string(body->norm_A) + "  D: " + std::to_string(body->deviation));
	}

	void SBSphereTreeCollisionAlgorithm::updateNode(SoftBody* body, SphereTreeNode* node)
	{
		if (node->update_tick < CollisionWorld::getCurrentTick())
		{
			//node->center = body->A * (node->center - body->originalCOM) + body->currentCOM;
			node->center = body->A * (node->original_center - body->original_com) + body->current_com;

			//node->radius = body->norm_A * node->radius + body->deviation;
			node->radius = body->norm_A * node->original_radius + body->deviation;

			node->update_tick = CollisionWorld::getCurrentTick();
		}		
	}

	float SBSphereTreeCollisionAlgorithm::calcANormLinear(SoftBody* body)
	{
		Eigen::Matrix3f m;

		m(0, 0) = body->A[0][0];
		m(0, 1) = body->A[1][0];
		m(0, 2) = body->A[2][0];

		m(1, 0) = body->A[0][1];
		m(1, 1) = body->A[1][1];
		m(1, 2) = body->A[2][1];

		m(2, 0) = body->A[0][2];
		m(2, 1) = body->A[1][2];
		m(2, 2) = body->A[2][2];

		return m.operatorNorm();
	}

	float SBSphereTreeCollisionAlgorithm::calcDeviationBasic(SoftBody*& body)
	{
		float deviation = 0.f;

		glm::vec3 d(0.f);
		glm::vec3 new_relative_pos(0.f);
		glm::vec3 transformed_init_pos(0.f);

		for (int i = 0; i < body->particles.size(); i++)
		{
			new_relative_pos = body->particles[i]->pos - body->current_com;
			transformed_init_pos = body->R * (body->particles[i]->init_pos - body->original_com);

			d = transformed_init_pos - new_relative_pos;

			float d_length = glm::length(d);

			deviation = deviation < d_length ? d_length : deviation;
		}

		return deviation;
	}

	float SBSphereTreeCollisionAlgorithm::calcDeviationLinear(SoftBody* body)
	{
		float deviation = 0.f;

		glm::vec3 d(0.f);
		glm::vec3 new_relative_pos(0.f);
		glm::vec3 transformed_init_pos(0.f);

		for (int i = 0; i < body->particles.size(); i++)
		{
			new_relative_pos = body->particles[i]->pos - body->current_com;
			transformed_init_pos = body->A * (body->particles[i]->init_pos - body->original_com);

			d = transformed_init_pos - new_relative_pos;

			float d_length = glm::length(d);

			deviation = deviation < d_length ? d_length : deviation;
		}
		return deviation;
	}

	void SBSphereTreeCollisionAlgorithm::testNodes(SoftBody* bodyA, SoftBody* bodyB, SphereTreeNode* nodeA, SphereTreeNode* nodeB, const glm::vec3 &gravity, const float &air_drag, std::unordered_map<SphereTreeNode*, std::vector<SphereTreeNode*>> &pcollisionsA, std::unordered_map<SphereTreeNode*, std::vector<SphereTreeNode*>> &pcollisionsB)
	{
		updateNode(bodyA, nodeA);
		updateNode(bodyB, nodeB);

		if ((nodeA->flag == SphereTreeNode::SphereFlag::INNER_NODE && nodeB->flag == SphereTreeNode::SphereFlag::INNER_NODE) ||
			(nodeA->flag == SphereTreeNode::SphereFlag::ROOT && nodeB->flag == SphereTreeNode::SphereFlag::ROOT))
		{
			if (sphereSphereIntersects(nodeA, nodeB))
			{
				for (int i = 0; i < nodeA->num_children; i++)
				{
					if (nodeA->children[i] != NULL)
					{
						for (int j = 0; j < nodeB->num_children; j++)
						{
							if (nodeB->children[j] != NULL)
								testNodes(bodyA, bodyB, nodeA->children[i], nodeB->children[j], gravity, air_drag, pcollisionsA, pcollisionsB);
						}
					}
				}			
			}
		}
		else if (nodeA->flag == SphereTreeNode::SphereFlag::LEAF && nodeB->flag == SphereTreeNode::SphereFlag::LEAF)
		{
			if (sphereSphereIntersects(nodeA, nodeB))
			{
					// Sphere - Sphere Kollision
				//....
				pcollisionsA[nodeA].push_back(nodeB);
				pcollisionsB[nodeB].push_back(nodeA);
			}
		}
		else if (nodeA->flag == SphereTreeNode::SphereFlag::LEAF && nodeB->flag == SphereTreeNode::SphereFlag::INNER_NODE)
		{
			for (int i = 0; i < nodeB->num_children; i++)
			{
				if (nodeB->children[i] != NULL)
					testNodes(bodyA, bodyB, nodeA, nodeB->children[i], gravity, air_drag, pcollisionsA, pcollisionsB);
			}
		}
		else if (nodeA->flag == SphereTreeNode::SphereFlag::INNER_NODE && nodeB->flag == SphereTreeNode::SphereFlag::LEAF)
		{
			for (int i = 0; i < nodeA->num_children; i++)
			{
				if (nodeA->children[i] != NULL)
					testNodes(bodyA, bodyB, nodeA->children[i], nodeB, gravity, air_drag, pcollisionsA, pcollisionsB);
			}
		}
		else
		{
			if ((nodeA->flag == SphereTreeNode::SphereFlag::ROOT && nodeB->flag == SphereTreeNode::SphereFlag::INNER_NODE) ||
				(nodeA->flag == SphereTreeNode::SphereFlag::INNER_NODE && nodeB->flag == SphereTreeNode::SphereFlag::ROOT))
			{
				// treat as error: add logger entry and cancel process
			}

			if ((nodeA->flag == SphereTreeNode::SphereFlag::ROOT && nodeB->flag == SphereTreeNode::SphereFlag::LEAF) ||
				(nodeA->flag == SphereTreeNode::SphereFlag::LEAF && nodeB->flag == SphereTreeNode::SphereFlag::ROOT))
			{
				// treat as error: add logger entry and cancel process
			}
		}
	}

	bool SBSphereTreeCollisionAlgorithm::sphereSphereIntersects(SphereTreeNode* nodeA, SphereTreeNode* nodeB)
	{
		glm::vec3 AB = glm::vec3(nodeA->center - nodeB->center);

		float AB_length_2 = (AB.x * AB.x) + (AB.y * AB.y) + (AB.z * AB.z);

		if (AB_length_2 <= (nodeA->radius + nodeB->radius)*(nodeA->radius + nodeB->radius))
			return true;

		return false;
	}

	bool SBSphereTreeCollisionAlgorithm::pointSphereIntersects(const glm::vec3 &pos, const SphereTreeNode* node)
	{
		glm::vec3 CP = pos - node->center;

		float CP_length_2 = (CP.x * CP.x) + (CP.y * CP.y) + (CP.z * CP.z);

		if (CP_length_2 <= (node->radius)*(node->radius))
			return true;

		return false;
	}

	void SBSphereTreeCollisionAlgorithm::sphereSphereCollision(SoftBody* bodyA, SoftBody* bodyB, std::unordered_map<SphereTreeNode*, std::vector<SphereTreeNode*>> &pcollisions, const glm::vec3 &gravity, const float &air_drag)
	{
		for (auto pair : pcollisions)
		{
			SphereTreeNode* nodeA = pair.first;
			SphereTreeNode* nodeB = NULL;
			float min_dist = FLT_MAX;

			for (int i = 0; i < pair.second.size(); i++)
			{
				glm::vec3 AB = nodeA->center - pair.second[i]->center;
				float dist = glm::length(AB);

				if (dist < min_dist)
				{
					min_dist = dist;
					nodeB = pair.second[i];
				}
			}

			Particle* pB_0 = bodyB->particles[nodeB->particle_ids[0]];
			Particle* pB_1 = bodyB->particles[nodeB->particle_ids[1]];
			Particle* pB_2 = bodyB->particles[nodeB->particle_ids[2]];

			glm::vec3 nodeB_ipos_0 = pB_0->intermediate_pos;
			glm::vec3 nodeB_ipos_1 = pB_1->intermediate_pos;
			glm::vec3 nodeB_ipos_2 = pB_2->intermediate_pos;

			glm::vec3 planeB_normal = glm::triangleNormal(nodeB_ipos_0, nodeB_ipos_1, nodeB_ipos_2);

			for (int i = 0; i < 3; i++)
			{
				Particle* pA = bodyA->particles[nodeA->particle_ids[i]];
				
				if (pA->narrowphase_test_state == Particle::NarrowPhaseTestState::UNTESTED)
				{
					if (glm::dot(planeB_normal, glm::vec3(pA->intermediate_pos - nodeB_ipos_0)) <= 0.f)
					{
						float one_third = 1.f / 3.f;

						glm::vec3 interpolated_velocity = pB_0->velocity * one_third + pB_1->velocity * one_third + pB_2->velocity * one_third;
						float interpolated_mass = pB_0->mass * one_third + pB_1->mass * one_third + pB_2->mass * one_third;
						glm::vec3 u1, u2;
						calcReflectedVelocity(pA->velocity, interpolated_velocity, planeB_normal, bodyA->restitution, pA->mass, interpolated_mass, u1, u2);

						{
							SoftBodyParticleCollisionData collision;
							collision.particle_id = nodeA->particle_ids[i];
							collision.contact_point = projectPointOntoPlane(pA->intermediate_pos, planeB_normal, nodeB_ipos_0);
							collision.reflected_velocity = u1;

							bodyA->collisions[nodeA->particle_ids[i]].push_back(collision);
							pA->narrowphase_test_state = Particle::NarrowPhaseTestState::TESTED;
						}
						/*{
							SoftBodyParticleCollisionData collision;
							collision.particle_id = nodeB->particle_ids[0];
							collision.contact_point = pB_0->intermediate_pos;
							collision.reflected_velocity = u2 * one_third;

							bodyB->collisions[nodeB->particle_ids[0]].push_back(collision);
						}*/
						/*{
							SoftBodyParticleCollisionData collision;
							collision.particle_id = nodeB->particle_ids[1];
							collision.contact_point = pB_1->intermediate_pos;
							collision.reflected_velocity = u2 * one_third;

							bodyB->collisions[nodeB->particle_ids[1]].push_back(collision);
						}
						{
							SoftBodyParticleCollisionData collision;
							collision.particle_id = nodeB->particle_ids[2];
							collision.contact_point = pB_2->intermediate_pos;
							collision.reflected_velocity = u2 * one_third;

							bodyB->collisions[nodeB->particle_ids[2]].push_back(collision);
						}*/
					}
				}
				
			}
		}

		/*glm::vec3 contact_normal = nodeB->center - nodeA->center;
		glm::vec3 nodeB_face_normal = calcSurfaceNormal(bodyB, nodeB);

		for (int i = 0; i < 3; i++)
		{
			Particle* p = bodyA->particles[nodeA->particle_ids[i]];

			if (pointSphereIntersects(p->intermediate_pos, nodeB))
			{

			}
		}*/
	}

	// not used at the moment
	void SBSphereTreeCollisionAlgorithm::checkParticles(SoftBody* bodyA, const SphereTreeNode* nodeA, SoftBody* bodyB, const SphereTreeNode* nodeB, const glm::vec3 &gravity, const float &air_drag)
	{
		// We are nodeA's triangle.
		glm::vec3 nodeA_triangle_normal = calcSurfaceNormal(bodyA, nodeA);
		glm::vec3 nodeB_triangle_normal = calcSurfaceNormal(bodyB, nodeB);

		// Disregard, if we are facing in the same general direction as nodeB's triangle / are perpendicular to it.
		// To collide with it, we would likely need to penetrate bodyB:
		/**********************************************************
		*		  /\
		*		 /--\ < this is us
		*		/----\
		*		  |
		*		  v [normalA]
		*
		*		  ^ [normalB]
		*	      | <<< (dot(normalA, normalB) < 0)
		*	  +-----------------------------+
		*  <--|                             | 
		*	  |            bodyB            | 
		*	  |                             | 
		*	  |                             | 
		*	  |                             |
		*	  +-----------------------------+
		*		  |
		*		  v
		************************************************************/
		if (glm::dot(nodeA_triangle_normal, nodeB_triangle_normal) < 0.f)
		{
			for (int i = 0; i < 3; i++)
			{
				Particle* pA = bodyA->particles[nodeA->particle_ids[i]];
				Particle* pB_0 = bodyB->particles[nodeB->particle_ids[0]];
				Particle* pB_1 = bodyB->particles[nodeB->particle_ids[1]];
				Particle* pB_2 = bodyB->particles[nodeB->particle_ids[2]];

				if (pA->narrowphase_test_state == Particle::NarrowPhaseTestState::UNTESTED)
				{
					if (pointSphereIntersects(pA->intermediate_pos, nodeB))
					{
						glm::vec3 pApB = pB_0->intermediate_pos - pA->intermediate_pos;

						// dot(pApB, nodeB_triangle_normal) >= 0 if particle is on illegal side of nodeB's triangle
						if (glm::dot(pApB, nodeB_triangle_normal) >= 0.f)
						{
							glm::vec3 plane_contact_point = projectPointOntoPlane(pA->intermediate_pos, nodeB_triangle_normal, pB_0->intermediate_pos);

							float u = 0.f;
							float v = 0.f;
							float w = 0.f;

							if (barycentric(plane_contact_point, pB_0->intermediate_pos, pB_1->intermediate_pos, pB_2->intermediate_pos, u, v, w))
							{
								glm::vec3 interpolated_velocity = pB_0->velocity * u + pB_1->velocity * v + pB_2->velocity * w;
								float interpolated_mass = pB_0->mass * u + pB_1->mass * v + pB_2->mass * w;
								glm::vec3 u1, u2;
								calcReflectedVelocity(pA->velocity, interpolated_velocity, nodeB_triangle_normal, bodyA->restitution, pA->mass, interpolated_mass, u1, u2);
								{
									SoftBodyParticleCollisionData collision;
									collision.particle_id = nodeA->particle_ids[i];
									collision.contact_point = plane_contact_point;
									collision.reflected_velocity = u1;

									bodyA->collisions[nodeA->particle_ids[i]].push_back(collision);
								}
								{
									SoftBodyParticleCollisionData collision;
									collision.particle_id = nodeB->particle_ids[0];
									collision.contact_point = pB_0->pos;
									collision.reflected_velocity = u2 * u;
									bodyB->collisions[nodeB->particle_ids[0]].push_back(collision);
								}
								{
									SoftBodyParticleCollisionData collision;
									collision.particle_id = nodeB->particle_ids[1];
									collision.contact_point = pB_1->pos;
									collision.reflected_velocity = u2 * v;
									bodyB->collisions[nodeB->particle_ids[1]].push_back(collision);
								}
								{
									SoftBodyParticleCollisionData collision;
									collision.particle_id = nodeB->particle_ids[2];
									collision.contact_point = pB_2->pos;
									collision.reflected_velocity = u2 * w;
									bodyB->collisions[nodeB->particle_ids[2]].push_back(collision);
								}


								pA->narrowphase_test_state = Particle::NarrowPhaseTestState::TESTED;
							}
						}
					}
				}	
			}
		}		
	}

	glm::vec3 SBSphereTreeCollisionAlgorithm::calcSurfaceNormal(const SoftBody* body, const SphereTreeNode* node)
	{
		/*glm::vec3 edge_u = glm::vec3(body->particles[node->particle_ids[1]]->intermediate_pos - body->particles[node->particle_ids[0]]->intermediate_pos);
		glm::vec3 edge_v = glm::vec3(body->particles[node->particle_ids[2]]->intermediate_pos - body->particles[node->particle_ids[0]]->intermediate_pos);

		glm::vec3 normal = glm::normalize(glm::cross(edge_u, edge_v));
		
		return normal;*/
		return glm::triangleNormal(body->particles[node->particle_ids[0]]->intermediate_pos, body->particles[node->particle_ids[1]]->intermediate_pos, body->particles[node->particle_ids[2]]->intermediate_pos);
	}

	glm::vec3 SBSphereTreeCollisionAlgorithm::projectPointOntoPlane(const glm::vec3 &point, const glm::vec3 &plane_normal, const glm::vec3 &plane_point)
	{
		return point - glm::dot(glm::vec3(point - plane_point), plane_normal) * plane_normal;
	}

	bool SBSphereTreeCollisionAlgorithm::segmentPlaneIntersects(
		const glm::vec3 &segment_start, 
		const glm::vec3 &segment_end, 
		const glm::vec3 &plane_normal, 
		const glm::vec3 &plane_point, 
		glm::vec3 &contact_point)
	{
		glm::vec3 segment_ab = segment_end - segment_start;
		float d = glm::dot(plane_normal, plane_point);
		float denom = glm::dot(plane_normal, segment_ab);
		if (glm::abs(denom) > FLT_EPSILON)
		{
			float t = (d - glm::dot(plane_normal, segment_start)) / denom;
			if (t >= 0.f && t <= 1.f)
			{
				contact_point = segment_start + t * segment_ab;
				return true;
			}
		}
		return false;
	}

	bool SBSphereTreeCollisionAlgorithm::barycentric(glm::vec3 &p, glm::vec3 &a, glm::vec3 &b, glm::vec3 &c, float &u, float &v, float &w)
	{
		glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
		float d00 = glm::dot(v0, v0);
		float d01 = glm::dot(v0, v1);
		float d11 = glm::dot(v1, v1);
		float d20 = glm::dot(v2, v0);
		float d21 = glm::dot(v2, v1);
		float denom = d00 * d11 - d01 * d01;
		v = (d11 * d20 - d01 * d21) / denom;
		if (v < 0.f || v > 1.f) return false;
		w = (d00 * d21 - d01 * d20) / denom;
		if (w < 0.f || w > 1.f) return false;
		u = 1.0f - v - w;
		if (u < 0.f || u > 1.f) return false;

		return true;
	}

	void SBSphereTreeCollisionAlgorithm::calcReflectedVelocity(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &normal, const float &restitution, const float &m1, const float &m2, glm::vec3 &res_v1, glm::vec3 &res_v2)
	{
		glm::vec3 v1_normal_component = (glm::dot(v1, normal)) * normal;
		glm::vec3 v1_tangential_component = v1 - v1_normal_component;
		glm::vec3 u1 = v1_normal_component;

		glm::vec3 v2_normal_component = (glm::dot(v2, normal)) * normal;
		glm::vec3 v2_tangential_component = v2 - v2_normal_component;
		glm::vec3 u2 = v2_normal_component;

		glm::vec3 w1 = (m1*u1 + m2 * u2 - m2 * (u1 - u2)) / (m1 + m2);
		glm::vec3 w2 = (m1*u1 + m2 * u2 - m1 * (u2 - u1)) / (m1 + m2);

		//glm::vec3 reflected_velocity = entry_velocity - 2.f * normal_component;

		res_v1 = (w1) + (v1_tangential_component * 0.9f);
		res_v2 = (w2) + (v2_tangential_component * 0.9f);
	}
}