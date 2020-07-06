#pragma once
#include "SceneNode.h"

namespace scenegraph
{

	class TransformationNode : public SceneNode
	{
	protected:
		glm::mat4x4 transformationMatrix;

	public:
		TransformationNode(glm::mat4x4 m);

		TransformationNode();
		~TransformationNode();

		void changeData(glm::mat4x4 m);

		virtual void update(StateBuffer *state) override;

		//Set Transformation matrix
		virtual void setTransformation(glm::mat4x4 trans) { transformationMatrix = trans; }
		//Get current Transformation matrix 
		virtual glm::mat4x4 getTransformation() { return transformationMatrix; }
	};

}