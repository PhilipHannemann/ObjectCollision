#include "stdafx.h"
#include "SBSphereTreeRayIntersectAlgorithm.h"
#include <EngineCore\IDebugRender.h>
#include <glm/gtx/normal.hpp>

#define EPSILON 0.00001f

namespace physics
{

	bool SBSphereTreeRayIntersectionAlgorithm::testRay(Ray ray, PickResult & data)
	{
		SoftBody* sb = static_cast<SoftBody*>(data.body);
		SphereTreeCollisionHull* hull = static_cast<SphereTreeCollisionHull*>(sb->collisionHull);

		prepareTraversal(sb);

		float lowest = INFINITY;
		
		bool b = testNodes(sb, hull->root, ray, data, lowest);

		//render::IDebugRender::getDebugRender()->drawSphere(hull->root->center, hull->root->radius, glm::vec4(1.0, 0.0, 0.0, 1.0));

		return b;
	}

	bool SBSphereTreeRayIntersectionAlgorithm::testNodes(SoftBody* sb, SphereTreeNode* node, Ray &ray, PickResult & data, float &lowest)
	{
		updateNode(sb, node);

		//render::IDebugRender::getDebugRender()->drawSphere(node->center, node->radius, glm::vec4(1.0, 0.0, 0.0, 1.0));

		if ((node->flag == SphereTreeNode::SphereFlag::INNER_NODE) || (node->flag == SphereTreeNode::SphereFlag::ROOT))
		{
			if (intersects(node, ray))
			{
				bool hit = false;

				for (int i = 0; i < SphereTreeNode::num_children; i++)
				{
					if (node->children[i] != nullptr)
						hit = testNodes(sb, node->children[i], ray, data, lowest) || hit;
				}

				return hit;
			}
		}
		
		else if (node->flag == SphereTreeNode::SphereFlag::LEAF)
		{
			if (intersects(node, ray))
			{
				if(debug_render)
					render::IDebugRender::getDebugRender()->drawSphere(node->center, node->radius, glm::vec4(1.0, 0.0, 0.0, 1.0));
				return checkParticles(sb, node, ray, data, lowest);
			}
		}

		return false;
	}

	void SBSphereTreeRayIntersectionAlgorithm::updateNode(SoftBody* body, SphereTreeNode* node)
	{
		//if (node->update_tick < simulation_tick_counter)
		{
			//node->center = body->A * (node->center - body->originalCOM) + body->currentCOM;
			node->center = body->A * (node->original_center - body->original_com) + body->current_com;

			//node->radius = body->norm_A * node->radius + body->deviation;
			node->radius = body->norm_A * node->original_radius + body->deviation;

			node->update_tick = simulation_tick_counter;
		}
	}

	void SBSphereTreeRayIntersectionAlgorithm::prepareTraversal(SoftBody* body)
	{
		switch (body->mode)
		{
		case BASIC:

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

	bool SBSphereTreeRayIntersectionAlgorithm::intersects(SphereTreeNode* node, Ray &ray)
	{
		float t0, t1; // solutions for t if the ray intersects

		glm::vec3 l = node->center - ray.start;

		float tca = glm::dot(l, ray.dir);
		float d2 = glm::dot(l, l) - tca * tca;

		if (d2 > node->radius * node->radius)
			return false;

		float thc = sqrt(node->radius * node->radius - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		if (t0 > t1) std::swap(t0, t1);

		if (t0 < 0) {
			t0 = t1;
			if (t0 < 0) return false; 
		}

		return true; //t0
	}

	bool SBSphereTreeRayIntersectionAlgorithm::checkParticles(SoftBody* body, SphereTreeNode* node, Ray &ray, PickResult & data, float &lowest)
	{
		Particle *p0 = body->particles[node->particle_ids[0]];
		Particle *p1 = body->particles[node->particle_ids[1]];
		Particle *p2 = body->particles[node->particle_ids[2]];

		glm::vec3 e1 = p1->pos - p0->pos;
		glm::vec3 e2 = p2->pos - p0->pos;
		glm::vec3 pvec = glm::cross(ray.dir, e2);
		float det = glm::dot(e1, pvec);

		if (det < EPSILON)
			return false;

		glm::vec3 tvec = ray.start - p0->pos;
		float u = glm::dot(tvec, pvec);

		if (u < 0 || u > det)
			return false;

		glm::vec3 qvec = glm::cross(tvec, e1);
		float v = glm::dot(ray.dir, qvec);

		if (v < 0 || u + v > det)
			return false;

		float t = glm::dot(e2, qvec) / det;
		
		if (lowest > t)
		{
			data.pos = glm::vec3(ray.start + t * ray.dir);
			data.normal = glm::triangleNormal(p0->pos, p1->pos, p2->pos);
			data.distance = t;
			lowest = t;
		}
		else
		{
			return false;	//allready closer hit found
		}

		return true;
	}

	float SBSphereTreeRayIntersectionAlgorithm::calcANormLinear(SoftBody* body)
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

	float SBSphereTreeRayIntersectionAlgorithm::calcDeviationLinear(SoftBody* body)
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

}