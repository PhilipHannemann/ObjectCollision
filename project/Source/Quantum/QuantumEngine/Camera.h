#pragma once
#include <EngineCore/MathHelpers.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <PhysicEngine\RayHelper.h>

namespace render 
{
	class Camera
	{
	public:
		Camera() {}
		virtual ~Camera() {}

	protected:
		//for 
		float aspect;
		float fov;
		float near;
		float far;

		//for cam
		glm::vec3 pos;
		glm::vec3 up;
		glm::vec3 dir;

		//cache
		util::MatrixCachingInverse projection;
		inline void recalcProj() 
		{
			projection = glm::perspective(fov, aspect, near, far);
		}

		//cache
		util::MatrixCachingInverse view;
		inline void recalcView()
		{
			glm::vec3 s(normalize(glm::cross(dir, up)));
			glm::vec3 u(glm::cross(s, dir));

			glm::mat4x4 result(1);
			result[0][0] = s.x;
			result[1][0] = s.y;
			result[2][0] = s.z;
			result[0][1] = u.x;
			result[1][1] = u.y;
			result[2][1] = u.z;
			result[0][2] = -dir.x;
			result[1][2] = -dir.y;
			result[2][2] = -dir.z;
			result[3][0] = -glm::dot(s, pos);
			result[3][1] = -glm::dot(u, pos);
			result[3][2] = glm::dot(dir, pos);
			view = result;
		}

		//helpers
		float i_width = 0;
		float i_height = 0;

	public:

		void setupPickRayHelper(int width, int height) 
		{
			i_width = 1.0f / width;
			i_height = 1.0f / height;
		}

		physics::Ray getPickRayTarget(int mx, int my) 
		{
			assert(i_width != 0.0);
			assert(i_height != 0.0);

			/*Calc uniform cursor pos*/
			float xn = ((float)mx * i_width) *2.0f - 1.0f;
			float yn = (1.0f-((float)my *i_height))*2.0f - 1.0f;

			//get Ray
			glm::vec4 rayTo = projection.getInverse()*glm::vec4(xn, yn, 1.0, 1.0);
			glm::vec4 rayFrom = projection.getInverse()*glm::vec4(xn, yn, -1.0, 1.0);
			
			//divw
			rayTo /= rayTo[3];
			rayFrom /= rayFrom[3];

			//view
			rayTo = view.getInverse() * rayTo;
			rayFrom = view.getInverse() * rayFrom;
			
			return physics::Ray(glm::normalize(glm::vec3(rayTo) - glm::vec3(rayFrom)), glm::vec3(rayFrom));
		}


		//getter
		float getFoV() { return fov; }
		float getAspect() { return aspect; }
		float getNearPlaneDistance() { return near; }
		float getFarPlaneDistance() { return far; }
		glm::vec3 getPosition() { return pos; }
		glm::vec3 getDirectionVec() { return dir; }
		glm::vec3 getUpVec() { return up; }


		//setter
		void setAspect(float aspect) 
		{
			this->aspect = aspect;
			recalcProj();
		}

		void setFoV(float fov)
		{
			this->fov = fov;
			recalcProj();
		}

		void setDistance(float near, float far)
		{
			this->near = near;
			this->far = far;
			recalcProj();
		}


		//
		void setPosition(glm::vec3 pos)
		{
			this->pos = pos;
			recalcView();
		}

		void setVectors(glm::vec3 dir, glm::vec3 up)
		{
			this->dir = glm::normalize(dir);
			this->up = glm::normalize(up);
			recalcView();
		}



	public:
		inline glm::mat4x4 getProjectionMatrix() { return projection.get(); }
		inline glm::mat4x4 getViewMatrix() { return view.get(); }
	};
	
}

