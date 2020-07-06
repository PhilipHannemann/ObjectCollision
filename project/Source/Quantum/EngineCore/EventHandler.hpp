#pragma once

namespace core {
	namespace events {

		template <class T>
		class EventHandler
		{
		public:

			virtual void onEvent(T*, int priority) = 0;


		};

	}
}