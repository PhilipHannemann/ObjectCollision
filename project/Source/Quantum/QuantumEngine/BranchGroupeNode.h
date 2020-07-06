//
//  CBranchGroupeNode.hpp
//  SceneGraph
//
//  Created by Philip Hannemann on 19.03.17.
//  Copyright © 2017 Philip Hannemann. All rights reserved.
//

#include "SceneNode.h"
namespace scenegraph
{

	class BranchGroupeNode : public SceneNode {

	public:
		BranchGroupeNode();

		~BranchGroupeNode();

		virtual void update(StateBuffer *state) override;
		//void render(render::DrawPass *pass);
	};
}