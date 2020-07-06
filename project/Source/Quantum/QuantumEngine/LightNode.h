#include "SceneNode.h"
#include "UBOGlobals.h"

namespace scenegraph
{

	class LightNode : public SceneNode
	{
	public:
		render::UBOGlobals::LightStruct ligth;

		LightNode();

		~LightNode();

		virtual void update(StateBuffer *state) override;

		//void render(render::DrawPass *pass);

	};

}