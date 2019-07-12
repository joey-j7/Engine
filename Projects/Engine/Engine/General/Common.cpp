#include "pch.h"

#include "Common.h"

std::string& ReplaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty() || from == to)
		return str;

	size_t start_pos = 0;

	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}

	return str;
}

bool Contains(const std::string& str, const std::string& search)
{
	if (str.empty() || search.empty())
		return search.empty();

	return str.find(search) != std::string::npos;
}