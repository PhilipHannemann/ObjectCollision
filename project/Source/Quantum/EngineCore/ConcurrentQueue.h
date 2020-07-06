#pragma once

#include <queue>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>

namespace util {

	template<typename Data>
	class ConcurrentQueue
	{
	private:
		std::queue<Data> buffer;
		SDL_mutex *mutex = SDL_CreateMutex();
		SDL_cond *cond = SDL_CreateCond();
		SDL_bool condition = SDL_FALSE;			//Daten sind bereit
		SDL_bool abortcond = SDL_FALSE;			//Abbruch

	public:
		//Add entry to buffer
		void push(Data const& data)
		{
			SDL_LockMutex(mutex);
			buffer.push(data);
			//Daten liegen bereit
			condition = SDL_TRUE;
			SDL_CondSignal(cond);
			SDL_UnlockMutex(mutex);
		}

		//Warten und Daten holen
		void wait_and_pop(Data& popped_value)
		{
			SDL_LockMutex(mutex);
			//Wait for data
			while (!abortcond && !condition && SDL_CondWait(cond, mutex) == 0) {
				continue;
			}

			if (buffer.empty())
			{
				//abbruch
				popped_value = "";
				condition = SDL_FALSE;
			}
			else
			{
				//daten holen
				popped_value = buffer.front();
				buffer.pop();

				//buffer leer zustand setzen
				if (buffer.empty())
				{
					condition = SDL_FALSE;
				}
			}
			SDL_UnlockMutex(mutex);
		}

		//Zustand zum beenden setzen
		void abort()
		{
			SDL_LockMutex(mutex);
			abortcond = SDL_TRUE;
			condition = SDL_TRUE;
			SDL_CondSignal(cond);
			SDL_UnlockMutex(mutex);
		}

		bool empty()
		{
			SDL_LockMutex(mutex);

			return buffer.empty();

			SDL_UnlockMutex(mutex);
		}

		void force_pop(Data& popped_value)
		{
			SDL_LockMutex(mutex);

			if (buffer.empty())
			{
				popped_value = "";
			}
			else
			{
				popped_value = buffer.front();
				buffer.pop();
			}
			SDL_UnlockMutex(mutex);
		}

		//cleanup
		~ConcurrentQueue()
		{
			SDL_DestroyMutex(mutex);
		}

	};

}