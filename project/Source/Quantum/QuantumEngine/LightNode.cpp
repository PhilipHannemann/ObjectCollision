#include "stdafx.h"
#include "LightNode.h"
#include "RenderCache.h"
#include "SceneGraph.h"

namespace scenegraph
{

	LightNode::LightNode() { }

	LightNode::~LightNode() { }

	void LightNode::update(StateBuffer *state) 
	{
		ligth.position = state->transform * glm::vec4(0.0, 0.0, 0.0, 1.0);
		state->rcache->globals->data.lights[state->rcache->globals->data.numligths ++] = this->ligth;


		SceneNode::update(state);
	}

}