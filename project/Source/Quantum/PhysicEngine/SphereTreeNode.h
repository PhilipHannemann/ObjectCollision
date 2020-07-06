#pragma once

#include <glm\glm.hpp>

#define _SphereTreeNode_NUM_CHILDREN_ 8

namespace physics
{
	class SphereTreeNode
	{
	public:
		enum SphereFlag
		{
			LEAF,
			INNER_NODE,
			ROOT
		};

		static const int num_children;

		int64_t update_tick;

		glm::vec3 center;
		float radius;

		glm::vec3 original_center;
		float original_radius;

		SphereTreeNode* parent;
		SphereTreeNode* children[_SphereTreeNode_NUM_CHILDREN_];

		SphereFlag flag;

		std::vector<int> particle_ids;

		SphereTreeNode(const SphereTreeNode &source);
		SphereTreeNode(const SphereTreeNode &source, glm::mat4x4 &trans, float &largestScale);

		SphereTreeNode(SphereTreeNode* parent, glm::vec3 center, float radius, SphereFlag flag) : particle_ids()
		{
			this->parent = parent;
			this->center = center;
			this->radius = radius;
			this->flag = flag;
			update_tick = -1;

			for (int i = 0; i < num_children; i++)
			{
				children[num_children] = NULL;
			}

			original_center = glm::vec3(0.f);
			original_radius = 0.f;
		}

		~SphereTreeNode();
	
		bool overlaps(SphereTreeNode* other) const;
	};
}
