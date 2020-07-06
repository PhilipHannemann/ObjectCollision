#include "stdafx.h"
#include "SphereTreeCollisionHull.h"
#include <glm\glm.hpp>
#include <glm\gtx\norm.hpp>

namespace physics
{
	float getLargestScale(const glm::mat4 &m)
	{
		glm::vec3 scale = glm::vec3(glm::length2(glm::vec3(m[0])), glm::length2(glm::vec3(m[1])), glm::length2(glm::vec3(m[2])));
		float max = glm::max(glm::max(scale.x, scale.y), scale.z);
		return glm::sqrt(max);
	}

	SphereTreeCollisionHull::SphereTreeCollisionHull(const SphereTreeCollisionHull &source)
	{
		this->root = new SphereTreeNode(*source.root);
	}

	SphereTreeCollisionHull::SphereTreeCollisionHull(const SphereTreeCollisionHull & source, glm::mat4x4 trans)
	{
		float ls = getLargestScale(trans);
		this->root = new SphereTreeNode(*source.root, trans, ls);
	}

	CollisionHull * SphereTreeCollisionHull::copy() const
	{
		// TODO: eigener copy constructor für deep copy! (root node beachten!)
		CollisionHull* copy = new SphereTreeCollisionHull(*this);

		return copy;
	}

	CollisionHull * SphereTreeCollisionHull::copyAndTransform(glm::mat4x4 trans) const
	{
		CollisionHull* copy = new SphereTreeCollisionHull(*this, trans);

		return copy;
	}

	SphereTreeCollisionHull::~SphereTreeCollisionHull()
	{
		delete root;
	}
}