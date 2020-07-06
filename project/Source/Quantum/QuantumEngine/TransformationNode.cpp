#include "stdafx.h"
#include "TransformationNode.h"
#include "SceneGraph.h"

namespace scenegraph
{

	TransformationNode::TransformationNode(glm::mat4x4 m) 
	{
		transformationMatrix = m;
	}

	TransformationNode::TransformationNode() 
	{
		transformationMatrix = glm::mat4(1);
	}

	TransformationNode::~TransformationNode() 
	{ 
	}

	void TransformationNode::changeData(glm::mat4x4 m)
	{
		transformationMatrix = m;
	}

	void TransformationNode::update(StateBuffer *state) 
	{	
		glm::mat4x4 t = state->transform;

		if (state->isTransformIdentity)
		{
			state->transform = transformationMatrix;
			state->isTransformIdentity = false;
			SceneNode::update(state);
			state->isTransformIdentity = true;
		}
		else
		{
			state->transform *= transformationMatrix;
			SceneNode::update(state);
		}

		state->transform = t;
	}

}