#pragma once
#include <iostream>
#include <iomanip>
//#include <limits>

namespace util {

	class Compression
	{
	private:
		Compression();
		~Compression();

	public:
		static void decompress(std::istream &is, std::ostream &os);
		static int compress(std::istream & is, std::ostream & os);
		static int compress(std::istream &is, std::ostream &os, int length);

	};
}

