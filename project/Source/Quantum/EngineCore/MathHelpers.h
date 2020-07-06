#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace util
{

	class MatrixCachingInverse
	{
		glm::mat4x4 data;
		glm::mat4x4 inv;
		bool dirty = true;
		
	public:
		inline const glm::mat4x4 & operator= (const glm::mat4x4 &m) { data = m; dirty = true; return m; }

		inline void set(const glm::mat4x4 &m) { data = glm::mat4x4(m); dirty = true; }

		inline glm::mat4x4 get() { return data; }

		inline glm::mat4x4 getInverse() 
		{  
			if (dirty)
			{
				inv = glm::inverse(data);
				dirty = false;
			}
			return inv;
		}
	
	};


	/*struct Ray 
	{
		glm::vec3 begin;
		glm::vec3 end;

		Ray(glm::vec3 b, glm::vec3 e) :begin(b), end(e) {};

		//btVector3 getBegin() { return vec_conv(begin); }
		//btVector3 getEnd() { return vec_conv(end); }
	};*/

}