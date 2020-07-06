#include "stdafx.h"
#include "AABB.h"

namespace util
{
	AABB::AABB() : min(glm::vec3(0.f)), max(glm::vec3(0.f)), dimensions{ 0.f }, surfaceArea(0.f), enclosedVolume(0.f)
	{
	
	}

	AABB::AABB(const glm::vec3& min, const glm::vec3&  max)
	{
		recalculateAABB(min, max);
	}

	AABB::~AABB()
	{

	}

	void AABB::recalculateAABB(const glm::vec3& min, const glm::vec3& max)
	{
		this->min = min;
		this->max = max;

		dimensions[0] = max.x - min.x;
		dimensions[1] = max.y - min.y;
		dimensions[2] = max.z - min.z;

		calcSurfaceArea();
		calcEnclosedVolume();
	}
}