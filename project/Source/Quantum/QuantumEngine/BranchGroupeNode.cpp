#include "stdafx.h"
//
//  CBranchGroupeNode.cpp
//  SceneGraph
//
//  Created by Philip Hannemann on 19.03.17.
//  Copyright © 2017 Philip Hannemann. All rights reserved.
//

#include "BranchGroupeNode.h"

namespace scenegraph
{

	BranchGroupeNode::BranchGroupeNode() { }

	BranchGroupeNode::~BranchGroupeNode() { }

	void BranchGroupeNode::update(StateBuffer *state) { SceneNode::update(state); }
}

