#pragma once
#include <EngineCore/AWindow.h>
#include <nanogui.h>
#include <PhysicEngine\SoftBody.h>
#include <PhysicEngine\PhysicsWorld.h>

namespace nanogui
{

	class SoftBodyWindow : public nanogui::AWindow, protected core::events::EventHandler<physics::CollsionBodyRemovedEventData>
	{
	public:
		Window * window;

		static physics::SoftBody *sb;
		static float alpha;
		static float beta;
		static float res;

		Slider *slider_a;
		Slider *slider_b;
		Slider *slider_r;

		core::events::EventRegistration<physics::CollsionBodyRemovedEventData> *myRegistartion;

		void update(physics::SoftBody *s);

		SoftBodyWindow(nanogui::Screen *gui_screen);
		~SoftBodyWindow();

	private:
		virtual void onEvent(physics::CollsionBodyRemovedEventData * ce, int priority) override
		{
			if (ce->body == sb)
				sb = nullptr;
		};
	};

}

