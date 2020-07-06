#pragma once
#include <cstring>

namespace util
{

	template <typename T>
	class TreeID
	{
	public:
		static int id;

	};
	template <typename T> int TreeID<T>::id(-1);



	class TreeRegistry
	{
	private:
		int *parray;
		int length;
		int next;
	public:

		TreeRegistry()
		{
			parray = new int[10];
			length = 10;
			next = 1;
			parray[0] = 0;
		}
		
		~TreeRegistry()
		{
			delete[] parray;
		}

		inline int getParent(int id)
		{
			return id < next ? parray[id] : 0;
		}

	private:
		int addNode(int parent)
		{
			if (next >= length)
			{
				int *c = new int[length + 10];
				std::memcpy(c, parray, length * sizeof(int));
				delete[]parray;
				parray = c;
				length += 10;
			}

			parray[next] = parent;
			next++;

			return (next - 1);
		}

	public:
		template <typename Target, typename Parent>
		int registerNode()
		{
			return TreeID<Target>::id = TreeID<Parent>::id + 1;
		}

	};
}