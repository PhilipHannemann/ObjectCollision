#include "stdafx.h"
#include "SBRBCollisionAlgorithm.h"
#include <EngineCore\Logger.h>
#include "PhysicsWorld.h"

namespace physics
{
	SBRBCollisionAlgorithm::SBRBCollisionAlgorithm()
	{

	}

	SBRBCollisionAlgorithm::~SBRBCollisionAlgorithm()
	{

	}

	// timeDelta der Simulation einfuegen!
	void SBRBCollisionAlgorithm::testPair(CollisionBody*& bodyA, CollisionBody*& bodyB, const glm::vec3 &gravity, const float &air_drag, const float &time_delta)
	{
		if (bodyA->type == body_index::SOFTBODY && bodyB->type == body_index::RIGIDBODY_KINEMATIC)
		{
			SoftBody* sb = static_cast<SoftBody*>(bodyA);
			SphereTreeCollisionHull * sb_hull = static_cast<SphereTreeCollisionHull*>(sb->collisionHull);
			AABBCollisionHull* rb_hull = static_cast<AABBCollisionHull*>(bodyB->collisionHull);

			prepareTraversal(sb);

			testNodeBox(sb, sb_hull->root, rb_hull, time_delta);
		}
		else if (bodyA->type == body_index::RIGIDBODY_KINEMATIC && bodyB->type == body_index::SOFTBODY)
		{
			SoftBody* sb = static_cast<SoftBody*>(bodyB);
			SphereTreeCollisionHull * sb_hull = static_cast<SphereTreeCollisionHull*>(sb->collisionHull);
			AABBCollisionHull* rb_hull = static_cast<AABBCollisionHull*>(bodyA->collisionHull);

			prepareTraversal(sb);
			
			testNodeBox(sb, sb_hull->root, rb_hull, time_delta);
		}
		else if (bodyA->type == body_index::SOFTBODY && bodyB->type == body_index::RIGIDBODY_DYNAMIC)
		{
			// TODO fertigstellen

			SoftBody* sb = static_cast<SoftBody*>(bodyA);
			RigidBody* rb_dynamic = static_cast<RigidBody*>(bodyB);

			prepareTraversal(sb);
		}
		else if (bodyA->type == body_index::RIGIDBODY_DYNAMIC && bodyB->type == body_index::SOFTBODY)
		{
			// TODO fertigstellen

			RigidBody* rb_dynamic = static_cast<RigidBody*>(bodyA);
			SoftBody* sb = static_cast<SoftBody*>(bodyB);

			prepareTraversal(sb);
		}
		else
		{
			// ERROR: logger entry here
		}
	}

	void SBRBCollisionAlgorithm::prepareTraversal(SoftBody*& body)
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

	void SBRBCollisionAlgorithm::updateNode(SoftBody*& body, SphereTreeNode*& node)
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

	float SBRBCollisionAlgorithm::calcANormLinear(SoftBody*& body)
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

	float SBRBCollisionAlgorithm::calcDeviationBasic(SoftBody*& body)
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

	float SBRBCollisionAlgorithm::calcDeviationLinear(SoftBody*& body)
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

			// TODO: deviation nur aus kollidierenden Partikeln des Objekts berechnen (wie?)
			float d_length = glm::length(d);

			deviation = deviation < d_length ? d_length : deviation;
		}

		return deviation;
	}

	void SBRBCollisionAlgorithm::testNodeBox(SoftBody*& bodyA, SphereTreeNode*& nodeA, const AABBCollisionHull* box, const float &time_delta)
	{
		updateNode(bodyA, nodeA);

		if (nodeA->flag == SphereTreeNode::SphereFlag::ROOT || nodeA->flag == SphereTreeNode::SphereFlag::INNER_NODE)
		{
			if (sphereBoxIntersects(nodeA, box))
			{
				for (int i = 0; i < SphereTreeNode::num_children; i++)
				{
					if (nodeA->children[i] != NULL)
						testNodeBox(bodyA, nodeA->children[i], box, time_delta);
				}
			}
		}
		else // nodeA->flag == SphereTreeNode::SphereFlag::LEAF
		{
			if (sphereBoxIntersects(nodeA, box))
			{
				for (int i = 0; i < 3; i++)
				{
					if (pointBoxIntersects(bodyA->particles[nodeA->particle_ids[i]]->intermediate_pos, box))
					{
						if (bodyA->particles[nodeA->particle_ids[i]]->narrowphase_test_state == Particle::NarrowPhaseTestState::UNTESTED)
						{
							glm::vec3 particle_ray = bodyA->particles[nodeA->particle_ids[i]]->velocity;//intermediate_pos - bodyA->particles[nodeA->particle_ids[i]]->pos;
							glm::vec3 particle_ray_origin = bodyA->particles[nodeA->particle_ids[i]]->pos;

							glm::vec3 contact_point(0.f);
							glm::vec3 normal(0.f);
									
							calcContactPointAndNormal(particle_ray, particle_ray_origin, box, contact_point, normal);

							glm::vec3 reflected_velocity = calcReflectedVelocity(bodyA->particles[nodeA->particle_ids[i]]->velocity, normal, bodyA->restitution);

							SoftBodyParticleCollisionData collision;
							collision.particle_id = nodeA->particle_ids[i];
							collision.contact_point = contact_point;
							collision.plane_normal = normal;
							collision.reflected_velocity = reflected_velocity;
							
							bodyA->collisions[nodeA->particle_ids[i]].push_back(collision);
							bodyA->particles[nodeA->particle_ids[i]]->narrowphase_test_state = Particle::NarrowPhaseTestState::TESTED;
						}						
					}			
				}
			}
		}
	}

	bool SBRBCollisionAlgorithm::sphereBoxIntersects(SphereTreeNode*& node, const AABBCollisionHull* box)
	{
		float dmin = 0.f;

		for (int i = 0; i < 3; i++) 
		{
			if (node->center[i] < box->min[i])
			{
				dmin += (node->center[i] - box->min[i]) * (node->center[i] - box->min[i]);
			}
			else
			{
				if (node->center[i] > box->max[i])
				{
					dmin += (node->center[i] - box->max[i]) * (node->center[i] - box->max[i]);
				}
			}
		}
		if (dmin <= (node->radius * node->radius)) return true;
		
		return false;
	}

	bool SBRBCollisionAlgorithm::pointBoxIntersects(const glm::vec3 &point, const AABBCollisionHull* box)
	{
		if (point.x > box->min.x && point.x < box->max.x &&
			point.y > box->min.y && point.y < box->max.y &&
			point.z > box->min.z && point.z < box->max.z)
			return true;

		return false;
	}

	void SBRBCollisionAlgorithm::calcContactPointAndNormal(const glm::vec3 &particle_ray, const glm::vec3 &particle_ray_origin, const AABBCollisionHull* box, glm::vec3 &contact_point, glm::vec3 &normal)
	{
		glm::vec3 inv = 1.0f / particle_ray;

		glm::vec3 t1 = (box->min - particle_ray_origin)*inv;
		glm::vec3 t2 = (box->max - particle_ray_origin)*inv;

		float tmin = glm::min(t1.x, t2.x);
		float tmax = glm::max(t1.x, t2.x);

		tmin = glm::max(tmin, glm::min(t1.y, t2.y));
		tmax = glm::min(tmax, glm::max(t1.y, t2.y));

		tmin = glm::max(tmin, glm::min(t1.z, t2.z));
		tmax = glm::min(tmax, glm::max(t1.z, t2.z));

		if (tmax >= tmin)
		{
			contact_point = particle_ray_origin + (tmin * particle_ray);
		}

		float min_dist = FLT_MAX;

		glm::vec3 aabb_center = (box->min + box->max) * 0.5f;
		glm::vec3 contact_point_local = contact_point - aabb_center;
		float distance;

		distance = glm::abs(box->dimensions[0] - glm::abs(contact_point_local.x));
		if (distance < min_dist)
		{
			min_dist = distance;
			normal = glm::sign(contact_point_local.x) * glm::vec3(1.f, 0.f, 0.f);
		}

		distance = glm::abs(box->dimensions[1] - glm::abs(contact_point_local.y));
		if (distance < min_dist)
		{
			min_dist = distance;
			normal = glm::sign(contact_point_local.y) * glm::vec3(0.f, 1.f, 0.f);
		}

		distance = glm::abs(box->dimensions[2] - glm::abs(contact_point_local.z));
		if (distance < min_dist)
		{
			min_dist = distance;
			normal = glm::sign(contact_point_local.z) * glm::vec3(0.f, 0.f, 1.f);
		}

		normal = glm::normalize(normal);

	}

	glm::vec3 SBRBCollisionAlgorithm::calcReflectedVelocity(const glm::vec3 &entry_velocity, const glm::vec3 &normal, const float &restitution)
	{		
		glm::vec3 normal_component = (glm::dot(entry_velocity, normal)) * normal;
		glm::vec3 tangential_component = entry_velocity - normal_component;

		//glm::vec3 reflected_velocity = entry_velocity - 2.f * normal_component;

		glm::vec3 reflected_velocity = (-normal_component * restitution) + (tangential_component * 0.6f);

		return reflected_velocity;
	}
}