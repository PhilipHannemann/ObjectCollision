#pragma once
#include <exception>
#include <stdexcept>

namespace render
{

	class LoadingResourceException : public std::runtime_error 
	{
	public:
		explicit LoadingResourceException(const std::string& type, const std::string& path)
			: std::runtime_error(("Can't load Resource (" + type + ") from file: " + path).c_str()){}
	};

}