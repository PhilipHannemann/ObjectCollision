#include "stdafx.h"
#include "SphereTreeNode.h"

namespace physics
{
	const int SphereTreeNode::num_children = _SphereTreeNode_NUM_CHILDREN_;

	SphereTreeNode::SphereTreeNode(const SphereTreeNode &source)
	{		
		this->center = source.center;
		this->radius = source.radius;
		this->flag = source.flag;
		this->original_center = source.original_center;
		this->original_radius = source.original_radius;
		this->update_tick = source.update_tick;
		this->particle_ids = source.particle_ids;
		this->parent = NULL;

		for (int i = 0; i < num_children; i++)
		{
			if (source.children[i] != NULL)
			{
				this->children[i] = new SphereTreeNode(*source.children[i]);
				this->children[i]->parent = this;
			}
			else
			{
				this->children[i] = NULL;
			}
		}
	}

	SphereTreeNode::SphereTreeNode(const SphereTreeNode & source, glm::mat4x4 &trans, float &largestScale)
	{
		this->center = glm::vec3(trans * glm::vec4(source.center, 1.0));
		this->radius = largestScale * source.radius;
		this->flag = source.flag;
		this->original_center = glm::vec3(trans * glm::vec4(source.original_center, 1.0));
		this->original_radius = largestScale * source.original_radius;
		this->update_tick = source.update_tick;
		this->particle_ids = source.particle_ids;
		this->parent = NULL;

		for (int i = 0; i < num_children; i++)
		{
			if (source.children[i] != NULL)
			{
				this->children[i] = new SphereTreeNode(*source.children[i], trans, largestScale);
				this->children[i]->parent = this;
			}
			else
			{
				this->children[i] = NULL;
			}
		}
	}


	SphereTreeNode::~SphereTreeNode()
	{
		for (int i = 0; i < num_children; i++)
		{
			if (this->children[i] != NULL)
				delete this->children[i];
		}
	}

	bool SphereTreeNode::overlaps(SphereTreeNode* other) const
	{
		glm::vec3 AB(center - other->center);

		float squared_distance = (AB.x * AB.x) + (AB.y * AB.y) + (AB.z * AB.z);

		return squared_distance < (radius + other->radius) * (radius + other->radius);
	}
}