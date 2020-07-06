#include "stdafx.h"
#include "GeometryNode.h"
#include "SceneGraph.h"
#include "RenderCache.h"
#include "RenderInstance.h"

namespace scenegraph
{

	DefaultGeometryNode::DefaultGeometryNode(render::RenderObject *ro)
	{
		renderinstance = new render::RenderInstance<render::DefaultShader::DefaultSSBO>(ro);
	}

	DefaultGeometryNode::~DefaultGeometryNode()
	{
		delete renderinstance;
	}

	void DefaultGeometryNode::update(StateBuffer *state)
	{
		auto t = renderinstance->getSSBO();
		t->data[renderinstance->ssboid].model_matrix = state->transform;
		t->data[renderinstance->ssboid].normal_matrix = glm::mat4(1);
		
		
		//renderinstance->~RenderInstance();

		//t->updateRange(renderinstance->ssboid, 1);

		state->rcache->enqueueInstance(renderinstance);

		SceneNode::update(state);
	}

	void DefaultGeometryNode::setColor(glm::vec4 color)
	{
		auto t = renderinstance->getSSBO();
		t->data[renderinstance->ssboid].color = color;
	}

}