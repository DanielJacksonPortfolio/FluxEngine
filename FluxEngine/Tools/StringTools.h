#pragma once
#include <string>
#include <algorithm>
#include <vector>

class StringTools
{
public:
	static std::wstring StandardToWide(std::string str);
	static std::string WideToStandard(std::wstring wstr);
	static std::string GetDirectoryFromPath(const std::string& filepath);
	static std::string GetFileExtension(const std::string& filename);
	static void SplitString(const std::string& string, std::vector<std::string>& container, char delim = ' ');
};

