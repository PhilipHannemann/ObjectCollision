#include "stdafx.h"
//
//  CAppearanceNode.cpp
//  SceneGraph
//
//  Created by Philip Hannemann on 19.03.17.
//  Copyright © 2017 Philip Hannemann. All rights reserved.
//

#include "AppearanceNode.h"

namespace scenegraph
{

	AppearanceNode::AppearanceNode() { }

	AppearanceNode::~AppearanceNode() { }

	void AppearanceNode::update(StateBuffer *state) {
		// apply color, transparancy, ...


		SceneNode::update(state);
	}

}
