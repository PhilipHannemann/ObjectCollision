#pragma once
#include <glm\glm.hpp>

namespace util
{
	class AABB
	{
	public:
		glm::vec3 min;
		glm::vec3 max;

		float dimensions[3];
		float surfaceArea;
		float enclosedVolume;

		AABB();
		AABB(const glm::vec3& min, const glm::vec3&  max);
		virtual ~AABB();

		virtual void recalculateAABB(const glm::vec3& min, const glm::vec3& max);

	private:

		void calcSurfaceArea()
		{
			surfaceArea = dimensions[0] * (dimensions[1] + dimensions[2]) + dimensions[1] * dimensions[2];
		}

		void calcEnclosedVolume() { enclosedVolume = dimensions[0] * dimensions[1] * dimensions[2]; }
	};
}