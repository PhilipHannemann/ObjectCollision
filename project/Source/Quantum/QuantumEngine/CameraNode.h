//
//  CCameraNode.hpp
//  SceneGraph
//
//  Created by Philip Hannemann on 19.03.17.
//  Copyright © 2017 Philip Hannemann. All rights reserved.
//

#include "SceneNode.h"
#include "Camera.h"
namespace scenegraph
{

	class CameraNode : public SceneNode
	{
	private:
		render::Camera *cam;

	public:
		CameraNode();
		~CameraNode();

		virtual void update(StateBuffer *state) override;
		//void render(render::DrawPass *pass);

	};

}