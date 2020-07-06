#pragma once
#include "Camera.h"
#include <EngineCore/Logger.h>

namespace render
{

	class CameraLookAt : public Camera
	{
	private:
		glm::vec3 center;
		float yaw;
		float pitch;
		float zoom;

		void recalcVec() 
		{
			/*if (yaw < -M_PI)
				yaw = -M_PI;
			
			if (yaw > M_PI)
				yaw = M_PI;*/

			if (zoom < 0.1f)
				zoom = 0.1f;

			if (pitch < 0.01f)
				pitch = 0.01f;

			if (pitch > M_PI/2.0f - 0.01f)
				pitch = (float) M_PI / 2.0f - 0.01f;

			glm::vec3 p;
			p.z = zoom * sin(pitch) * cos(yaw);
			p.x = zoom * sin(pitch) * sin(yaw);
			p.y = zoom * cos(pitch);
			pos = p+center;

			dir = glm::normalize(center - pos);
			//glm::lookAt
			//up = glm::normalize(glm::cross(dir, ) );

			recalcView();
		}

	public:
		CameraLookAt();
		virtual ~CameraLookAt();

		void setAll(glm::vec3 c, float y, float p, float z) 
		{
			center = c;
			yaw = y;
			pitch = p;
			zoom = z;
			recalcVec();
		}

		void addCenter(glm::vec3 ac) 
		{ 
			center += ac; 		
			recalcVec();
		}

		void addCamX(float ac)
		{
			glm::vec3 d = dir;
			d.y = 0;
			d = glm::normalize(d);
			center += d * ac;

			recalcVec();
		}

		void addCamZ(float ac)
		{
			glm::vec3 d = dir;
			d.y = d.x;
			d.x = d.z;
			d.z = -d.y;
			d.y = 0;
			d = glm::normalize(d);
			center += d * ac;

			recalcVec();
		}

		void setCenter(glm::vec3 c) 
		{ 
			center = c; 
			recalcVec();
		}


		void addYaw(float y)
		{
			yaw += y;			
			recalcVec();
		}

		void setYaw(float y)
		{
			yaw = y;
			recalcVec();
		}

		void addPitch(float p)
		{
			pitch += p;
			recalcVec();
		}

		void setPitch(float p)
		{
			pitch = p;
			recalcVec();
		}

		void addZoom(float z)
		{
			zoom += z;
			recalcVec();
		}

		void setZoom(float z)
		{
			zoom = z;
			recalcVec();
		}

	};



}
