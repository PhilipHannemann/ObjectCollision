#include "stdafx.h"
#include "BroadphaseAABB.h"
#include "BVHNode.h"

namespace physics
{

	bool BroadphaseAABB::overlaps(const BroadphaseAABB& other) const
	{
		return (min.x < other.max.x) && (max.x > other.min.x) &&
		       (min.y < other.max.y) && (max.y > other.min.y) &&
			   (min.z < other.max.z) && (max.z > other.min.z);
	}

	bool BroadphaseAABB::contains(const BroadphaseAABB& other) const
	{
		return (min.x <= other.min.x) && (max.x >= other.max.x) &&
		  	   (min.y <= other.min.y) && (max.y >= other.max.y) &&
			   (min.z <= other.min.z) && (max.z >= other.max.z);
	}

	float BroadphaseAABB::getGrowth(const BroadphaseAABB& other)
	{
		float minX = min.x < other.min.x ? min.x : other.min.x;
		float minY = min.y < other.min.y ? min.y : other.min.y;
		float minZ = min.z < other.min.z ? min.z : other.min.z;

		float maxX = max.x > other.max.x ? max.x : other.max.x;
		float maxY = max.y > other.max.y ? max.y : other.max.y;
		float maxZ = max.z > other.max.z ? max.z : other.max.z;

		BroadphaseAABB newBox(glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ));

		return newBox.surfaceArea - surfaceArea;
	}

	void BroadphaseAABB::recalculateAABB(const glm::vec3& min, const glm::vec3& max)
	{
		this->min = min;
		this->max = max;

		dimensions[0] = max.x - min.x;
		dimensions[1] = max.y - min.y;
		dimensions[2] = max.z - min.z;

		surfaceArea = dimensions[0] * (dimensions[1] + dimensions[2]) + dimensions[1] * dimensions[2];

		enclosedVolume = dimensions[0] * dimensions[1] * dimensions[2];

		//if (owner->parent != NULL)
		//	owner->parent->recalcBoundingVolume();
	}
}