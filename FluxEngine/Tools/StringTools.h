#pragma once
#include <string>
#include <algorithm>

class StringTools
{
public:
	static std::wstring StandardToWide(std::string str);
	static std::string WideToStandard(std::wstring wstr);
	static std::string GetDirectoryFromPath(const std::string& filepath);
	static std::string GetFileExtension(const std::string& filename);
};

