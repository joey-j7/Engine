#include "pch.h"

#include "Common.h"

String& ReplaceAll(String& str, const String& from, const String& to)
{
	if (from.empty() || from == to)
		return str;

	size_t start_pos = 0;

	while ((start_pos = str.find(from, start_pos)) != String::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}

	return str;
}

bool Contains(const String& str, const String& search)
{
	if (str.empty() || search.empty())
		return search.empty();

	return str.find(search) != String::npos;
}