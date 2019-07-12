#pragma once

#include <string>

#define TOSTR(var) #var
#define TOSTRING(var) TOSTR(var)

std::string& ReplaceAll(std::string& str, const std::string& from, const std::string& to);
bool Contains(const std::string& str, const std::string& search);