#pragma once

#include <string>

typedef std::string String;

#define TOSTR(var) #var
#define TOSTRING(var) TOSTR(var)

String& ReplaceAll(String& str, const String& from, const String& to);
bool Contains(const String& str, const String& search);