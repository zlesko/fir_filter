#pragma once

#include <exception>
#include <stdexcept>

class accuracy_error : public std::runtime_error
{
public:
	accuracy_error(const char* what_arg);
};

class config_error : public std::runtime_error
{
public:
	config_error(const char* what_arg);
};

class parameter_error : public std::runtime_error
{
public:
	parameter_error(const char* what_arg);
};
