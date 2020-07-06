#pragma once
#include "SceneNode.h"
#include "SoftBodyModelWrapper.h"
#include "RenderObject.h"
#include "SoftBodyShader.h"
#include <PhysicEngine\PhysicsWorld.h>

namespace scenegraph
{

	class SoftBodyCompactNode : public SceneNode
	{
	private:
		render::RenderInstance<render::SoftBodyShader::SoftBodySSBO> *renderinstance; //todo
		
		int sbdid;
		physics::DynamicsWorld *cache_world;
		loader::SoftBodyModelWrapper *sbmw;
		bool added;

	public:
		physics::SoftBody *softbody;

		SoftBodyCompactNode(physics::DynamicsWorld *world, loader::SoftBodyModelWrapper *ci, float mass, glm::mat4 transform, physics::deformation_mode dmode);
		
		virtual ~SoftBodyCompactNode();

		virtual void addedToSceneGraph() override;

		virtual void update(StateBuffer *state) override;

		void setColor(glm::vec4 color);

		glm::vec4 getColor();

		//void render(render::DrawPass *pass) override;

		//render::RenderObject* getRenderObject() { return ro; }

	};

}