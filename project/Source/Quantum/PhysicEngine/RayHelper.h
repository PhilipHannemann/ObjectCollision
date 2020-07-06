#pragma once
#include <glm\glm.hpp>
#include "CollisionBody.h"

namespace physics
{
	struct Ray
	{
		glm::vec3 dir;
		glm::vec3 start;

		Ray(glm::vec3 d, glm::vec3 s) :dir(d), start(s) {}
	};

	struct PickResult
	{
		float distance;
		CollisionBody *body;
		glm::vec3 pos;
		glm::vec3 normal;

		bool hasHit() { return distance >= 0.0f; }

		PickResult(float d, CollisionBody* b) :distance(d), body(b) {}
		PickResult() :distance(-1.0f), body(nullptr) {}
	};

}