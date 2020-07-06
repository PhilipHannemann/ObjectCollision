#pragma once

#include "CollisionHull.h"
#include "SphereTreeNode.h"
#include "Particle.h"

namespace physics
{
	 /* Might want to rename this class + its header, source file and SphereTreeNode, as it's no general purpose
	    implementation for collision hulls based on sphere trees. This class is aimed at soft bodies. */
	class SphereTreeCollisionHull : public CollisionHull
	{
	public:
		SphereTreeNode* root;

		SphereTreeCollisionHull(const SphereTreeCollisionHull &source);
		SphereTreeCollisionHull(const SphereTreeCollisionHull &source, glm::mat4x4 trans);
		SphereTreeCollisionHull(SphereTreeNode* node) { root = node; }
		virtual CollisionHull* copy() const override;
		virtual CollisionHull* copyAndTransform(glm::mat4x4 trans) const override;
		virtual ~SphereTreeCollisionHull();
		virtual util::AABB getAABB()
		{
			return util::AABB(root->center - root->radius, root->center + root->radius);
		}
	};
}