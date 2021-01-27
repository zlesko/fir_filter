#include "errors_custom.h"

accuracy_error::accuracy_error(const char* what_arg) :
	std::runtime_error(what_arg) {}

config_error::config_error(const char* what_arg) :
	std::runtime_error(what_arg) {}

parameter_error::parameter_error(const char* what_arg) :
	std::runtime_error(what_arg) {}
