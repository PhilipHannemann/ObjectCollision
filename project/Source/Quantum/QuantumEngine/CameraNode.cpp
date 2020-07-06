#include "stdafx.h"
//
//  CCameraNode.cpp
//  SceneGraph
//
//  Created by Philip Hannemann on 19.03.17.
//  Copyright © 2017 Philip Hannemann. All rights reserved.
//

#include "CameraNode.h"

namespace scenegraph
{
	CameraNode::CameraNode() { }

	CameraNode::~CameraNode() { }

	void CameraNode::update(StateBuffer *state) {
		// Draw our camera here!

		SceneNode::update(state);
	}
}
