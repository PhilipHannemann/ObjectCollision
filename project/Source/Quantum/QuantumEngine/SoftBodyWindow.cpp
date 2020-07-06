#include "stdafx.h"
#include "SoftBodyWindow.h"
#include <nanogui.h>


namespace nanogui
{
	float SoftBodyWindow::alpha = 0.5f;
	float SoftBodyWindow::beta = 0.2f;
	float SoftBodyWindow::res = 0.2f;
	physics::SoftBody *SoftBodyWindow::sb = nullptr;

	void SoftBodyWindow::update(physics::SoftBody * s)
	{
		sb = s;
		if (sb != nullptr)
		{
			slider_a->setValue(sb->alpha);
			slider_a->callback()(sb->alpha);
			slider_b->setValue(sb->beta);
			slider_b->callback()(sb->beta);
			slider_r->setValue(sb->restitution);
			slider_r->callback()(sb->restitution);
		}
	}

	SoftBodyWindow::SoftBodyWindow(nanogui::Screen * gui_screen) :AWindow(gui_screen)
	{
		window = new Window(gui_screen, "Softbody Config");
		window->setPosition(Vector2i(15, 380));
		window->setLayout(new GroupLayout());

		(new Label(window, "Alpha", "sans-bold"));
		{
			alpha = 0.5f;

			Widget *panel = new Widget(window);
			panel->setLayout(new BoxLayout(Orientation::Horizontal,
				Alignment::Middle, 0, 20));

			slider_a = new Slider(panel);
			slider_a->setValue(alpha);
			slider_a->setFixedWidth(160);

			TextBox *textBox = new TextBox(panel);
			textBox->setFixedSize(Vector2i(60, 25));
			textBox->setValue("50");
			textBox->setUnits("%");
			slider_a->setCallback([textBox](float value) {
				textBox->setValue(std::to_string((int)(value * 100)));
				alpha = value;
				
				if (sb != nullptr)
					sb->alpha = alpha;
			});
			slider_a->setFinalCallback([&](float value) {
				//cout << "Final slider value: " << (int)(value * 100) << endl;
			});
			textBox->setFixedSize(Vector2i(60, 25));
			textBox->setFontSize(20);
			textBox->setAlignment(TextBox::Alignment::Right);
		} 
		(new Label(window, "Beta", "sans-bold"));
		{
			beta = 0.2f;

			Widget *panel = new Widget(window);
			panel->setLayout(new BoxLayout(Orientation::Horizontal,
				Alignment::Middle, 0, 20));

			slider_b = new Slider(panel);
			slider_b->setValue(beta);
			slider_b->setFixedWidth(160);

			TextBox *textBox = new TextBox(panel);
			textBox->setFixedSize(Vector2i(60, 25));
			textBox->setValue("20");
			textBox->setUnits("%");
			slider_b->setCallback([textBox](float value) {
				textBox->setValue(std::to_string((int)(value * 100)));
				beta = value;

				if (sb != nullptr)
					sb->beta = beta;
			});
			slider_b->setFinalCallback([&](float value) {
				//cout << "Final slider value: " << (int)(value * 100) << endl;
			});
			textBox->setFixedSize(Vector2i(60, 25));
			textBox->setFontSize(20);
			textBox->setAlignment(TextBox::Alignment::Right);
		}
		(new Label(window, "Restitution", "sans-bold"));
		{
			res = 0.2f;

			Widget *panel = new Widget(window);
			panel->setLayout(new BoxLayout(Orientation::Horizontal,
				Alignment::Middle, 0, 20));

			slider_r = new Slider(panel);
			slider_r->setValue(res);
			slider_r->setFixedWidth(160);

			TextBox *textBox = new TextBox(panel);
			textBox->setFixedSize(Vector2i(60, 25));
			textBox->setValue("20");
			textBox->setUnits("%");
			slider_r->setCallback([textBox](float value) {
				textBox->setValue(std::to_string((int)(value * 100)));
				res = value;

				if (sb != nullptr)
					sb->restitution = res;
			});
			slider_r->setFinalCallback([&](float value) {
				//cout << "Final slider value: " << (int)(value * 100) << endl;
			});
			textBox->setFixedSize(Vector2i(60, 25));
			textBox->setFontSize(20);
			textBox->setAlignment(TextBox::Alignment::Right);
		}

		myRegistartion = core::events::Event<physics::CollsionBodyRemovedEventData>::addHandler(this, 10);
	}

	SoftBodyWindow::~SoftBodyWindow()
	{
		delete myRegistartion;
	}

}