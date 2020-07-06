#pragma once
#include <exception>
#include <string>

/*
File contianing all new exception types of the basic Features
*/

//don't catch this execption; it should shutdown the application
class FatalException : public std::exception
{
public:
	typedef std::exception _Mybase;
	explicit FatalException(const std::string& _Message) :_Mybase(_Message.c_str()) {}
	explicit FatalException(const char *_Message) : _Mybase(_Message) {}
};

class IncompatibleTypeException : public std::exception
{
public:
	typedef std::exception _Mybase;
	explicit IncompatibleTypeException(const std::string& _Message) :_Mybase(_Message.c_str()) {}
	explicit IncompatibleTypeException(const char *_Message) : _Mybase(_Message) {}
};

class EmptyEntryException : public std::exception
{
public:
	typedef std::exception _Mybase;
	explicit EmptyEntryException(const std::string& _Message) :_Mybase(_Message.c_str()) {}
	explicit EmptyEntryException(const char *_Message) : _Mybase(_Message) {}
};

class ModelLoadingException : public std::exception
{
public:
	typedef std::exception _Mybase;
	explicit ModelLoadingException(const std::string& _Message) :_Mybase(_Message.c_str()) {}
	explicit ModelLoadingException(const char *_Message) : _Mybase(_Message) {}
};

class TextureLoadingException : public std::exception
{
public:
	typedef std::exception _Mybase;
	explicit TextureLoadingException(const std::string& _Message) :_Mybase(_Message.c_str()) {}
	explicit TextureLoadingException(const char *_Message) : _Mybase(_Message) {}
};

class SceneGraphConstructingException : public std::exception
{
public:
	typedef std::exception _Mybase;
	explicit SceneGraphConstructingException(const std::string& _Message) :_Mybase(_Message.c_str()) {}
	explicit SceneGraphConstructingException(const char *_Message) : _Mybase(_Message) {}
};

