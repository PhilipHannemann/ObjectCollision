#pragma once

namespace core {
	namespace events {

		class EventDataSimple
		{
		public:

			EventDataSimple()
			{
			}

		};


		class EventDataCancelable
		{
		private:
			bool canceled;

		public:
			EventDataCancelable() : canceled(false)
			{

			}

			void cancel()
			{
				canceled = true;
			}

			bool getCanceled()
			{
				return canceled;
			}
		};

	}
}