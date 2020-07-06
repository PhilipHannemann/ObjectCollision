#pragma once
#include "IVertexData.h"
#include "RenderConstants.h"
#include <glm\glm.hpp>
#include "RenderObject.h"

#if 0

namespace render
{
	class RenderObjectMaterial : public RenderObject, private util::TreeID<RenderObjectMaterial>
	{
	public:

		glm::vec4 color;

		RenderObjectMaterial()
		{
			if(((util::TreeID<RenderObjectMaterial>*)this)->id == -1)
				constant::renderObjectTypes->registerNode<RenderObjectMaterial, RenderObject>();
		}

		virtual ~RenderObjectMaterial() {}

		virtual int getTreeId()
		{
			return ((util::TreeID<RenderObjectMaterial>*)this)->id;
		}

		void setColor(glm::vec4 c)
		{
			color = c;
		}

		virtual RenderObject *clone() { return new RenderObjectMaterial(*this); }

	};
}

#endif