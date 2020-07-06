#pragma once
#include "SceneNode.h"
#include "RenderInstance.h"
#include "DefaultShader.h"

namespace render { class RenderObject; class RenderObject; }

namespace scenegraph
{

	class DefaultGeometryNode : public SceneNode
	{
	protected:
		render::RenderInstance<render::DefaultShader::DefaultSSBO> *renderinstance;

	public:
		DefaultGeometryNode(render::RenderObject *ro);
		~DefaultGeometryNode();

		virtual void update(StateBuffer *state) override;

		void setColor(glm::vec4 color);

		render::RenderInstance<render::DefaultShader::DefaultSSBO>* getRenderInstance()
		{
			return renderinstance;
		}

	};

}