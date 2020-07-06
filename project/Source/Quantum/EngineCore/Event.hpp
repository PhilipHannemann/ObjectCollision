#pragma once

/*
---- Full Example ----

-- Creating Event Types --
*presets are: "EventData" and "EventDataCancelable"

	class ExampleEventData : public EventDataCancelable
	{
	public:
		ExampleEventData(...){....}
	};

-- Create Event Handler --
*handlers for multiple events are possible

	class MyExampleClass : public EventHandler<ExampleEventData>
	{
		virtual void onEvent(ExampleEventData * ce, int priority) override
		{
			{....}
			//ce->cancel();		//to cancel the Event
		};
	};

-- Subscribe Event --
*you sould store the EventRegistration<T> objects
*registration is only for the passed instance
*one class can be registerd multiple times
*one instance can be registerd multiple times
*you can register multiple times with same priority
*lower priority is run first

	MyExampleClass *myExampleObject = new MyExampleClass(...);
	EventRegistration<ExampleEventData> *myRegistartion = Event<ExampleEventData>::addHandler(myExampleObject, priority);

-- Unsubscribe Event --
*this will NOT delete "myExampleObject"

	Event<ExampleEventData>::removeHandler(myRegistartion);

-- Fire Event --

	Event<ExampleEventData>::fireEvent(new ExampleEventData(...));

-- CleanUp --
*this automatically removes "myRegistartion"
*you must do this for EVERY event type (EventData)

	delete myExampleObject;
	Event<ExampleEventData>::cleanup();
	

-- Tipps to prevent issues --
*never use an Event<T> after "cleanup()"
*don't try to remove an EventRegistration<T> after "cleanup()"
*never delete an object, befor all EventRegistration<T> for it are removed 
*if you put remove of EventRegistration<T> in destructors run this before "cleanup" !
*never delete the EventData object in an EventHandler
*never delete an EventRegistration<T>
*"fireEvent" calls "delete" for the EventData, if finished; you can disable this,using: Event<T>::setDeleteEventData(false);

*/

#include <list>
#include <iomanip>
#include "EventHandler.hpp"
#include "EventData.hpp"

/**
* A EventRegistration Instance is created by adding an Event
* It schould be stored for unregistering and cleanup
* unregistering is done by "delete" of the Instance
* 
* @author Wugi
* @version 3.2
*/

namespace core {
	namespace events {

		template <class T>
		class EventRegistration
		{
			//DataType for the List and Iterator
			typedef std::list<EventRegistration *> ListenerList;
			//typedef typename ListenerList::iterator ListenerListIterator;

		private:

			EventHandler<T> *handler;
			//ListenerList *listeners;
			int priority;

		public:
			//Internal Use Only
			EventRegistration(EventHandler<T> *h, int prio) : handler(h), priority(prio)
			{
			}

			//delete an unregister
			~EventRegistration()
			{
				//if (listeners != nullptr)
				//	listeners->remove(this);
			}

			//generic getter
			inline EventHandler<T> * const getHandler()
			{
				return handler;
			}

			//generic getter
			inline int getPriortity()
			{
				return priority;
			}

		};


		/**
		* Template for typical liniar Eventbus
		* For every used Template Parameter a indipendent EventHandlering System (including it's own listeners list) is created
		*
		* @param T Class of the EventData; base-class: EventDataSimple or EventDataCancelable
		* @author Wugi
		* @version 3.2
		*/
		template <class T>
		class Event
		{
		private:

			//DataType for the List and Iterator -- same as in EventRegistration!!!
			typedef std::list<EventRegistration<T> *> ListenerList;
			typedef typename ListenerList::iterator ListenerListIterator;

			//the List itself
			static ListenerList *listeners;
			static bool deleteEventData;

		public:

			static void setDeleteEventData(bool b)
			{
				deleteEventData = b;
			}

			/**
			* Adds an Event Handler to the list
			*
			* @param eh Instance of an Class that overrides the onEvent of an EventHandler<T>
			* @param priority prioritys value [-100, 100], values < 0 are cancelable (if T extends EventDataCancelable); lower will execute first
			* @return EventRegistration<T>* instance of the Event, used for removal
			*/
			static EventRegistration<T>* addHandler(EventHandler<T> *eh, int priority)
			{
				EventRegistration<T> * reg = new EventRegistration<T>(eh, priority);


				ListenerListIterator target = listeners->end();

				for (ListenerListIterator i = listeners->begin(); i != listeners->end(); ++i)
				{
					if ((*i)->getPriortity() > priority)
					{
						target = i;
						break;
					}
				}

				listeners->insert(target, reg);

				return reg;
			}

			static void removeHandler(EventRegistration<T> *eh)
			{
				for (ListenerListIterator i = listeners->begin(); i != listeners->end(); ++i)
				{
					if ((*i) == eh)
					{
						listeners->erase(i);
						delete eh;
						break;
					}
				}
			}

			/**
			* Fires an Event and execute all listeners (until Event is canceled)
			*
			* @param data The EventData must extend EventDataCancelable (and T)
			*/
			template<typename N, typename std::enable_if<std::is_base_of<EventDataCancelable, N>::value && std::is_base_of<T, N>::value, int>::type = 0>
			static void fireEvent(N *data)
			{
				EventDataCancelable *edc = static_cast<EventDataCancelable *>(data);

				for (EventRegistration<T> * reg : *listeners)
				{
					if (edc->getCanceled())
						break;

					reg->getHandler()->onEvent(data, reg->getPriortity());
				}

				if (deleteEventData)
					delete data;
			}

			/**
			* Fires an Event and execute all listeners
			*
			* @param data The EventData must extend EventDataSimple (and T)
			*/
			template<typename N, typename std::enable_if<std::is_base_of<EventDataSimple, N>::value && std::is_base_of<T, N>::value, int>::type = 0>
			static void fireEvent(N *data)
			{
				for (EventRegistration<T> * reg : *listeners)
				{
					reg->getHandler()->onEvent(data, reg->getPriortity());
				}

				if (deleteEventData)
					delete data;
			}


#ifdef DEBUG
			static void print()
			{
				std::cout << "Handlers registered:" << listeners->size() << std::endl;

				for (EventRegistration<T> * reg : *listeners)
				{
					std::cout << std::setw(4) << reg->getPriortity() << " @ " << reg->getHandler() << std::endl;
				}

				std::cout << std::endl;
			}
#endif // DEBUG

			/**
			* Use this at Programm-Shutdown for corret memory cleanup
			*/
			static void cleanup()
			{
				while (!listeners->empty())
				{
					delete *listeners->begin();
					listeners->erase(listeners->begin());
				}

				delete listeners;
			}

		};

		//datastore for static members
		template <class T>
		std::list<EventRegistration<T> *> *Event<T>::listeners = new std::list<EventRegistration<T> *>();

		template <class T>
		bool Event<T>::deleteEventData = true;


	}
}