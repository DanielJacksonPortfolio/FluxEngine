#include "StringTools.h"

std::wstring StringTools::StandardToWide(std::string str)
{
	return std::wstring(str.begin(), str.end());
}
std::string StringTools::WideToStandard(std::wstring wstr)
{
	return std::string(wstr.begin(), wstr.end());
	
}

std::string StringTools::GetDirectoryFromPath(const std::string& filepath)
{
	size_t offset1 = filepath.find_last_of('\\');
	size_t offset2 = filepath.find_last_of('/');
	if (offset1 == std::string::npos && offset2 == std::string::npos) //No Directory
		return "";
	if (offset1 == std::string::npos) //Directory (\\)
		return filepath.substr(0, offset2);
	if (offset2 == std::string::npos) //Directory (/)
		return filepath.substr(0, offset1);

	return filepath.substr(0, std::max(offset1, offset2)); //Directory (Last slash)
}

std::string StringTools::GetFileExtension(const std::string& filename)
{
	size_t offset = filename.find_last_of('.');
	if (offset == std::string::npos) //No extension
		return{ };

	return std::string(filename.substr(offset + 1)); //Extension
}

void StringTools::SplitString(const std::string& string, std::vector<std::string>& container, char delim)
{
	std::size_t current, previous = 0;
	current = string.find(delim);
	while (current != std::string::npos) {
		container.push_back(string.substr(previous, current - previous));
		previous = current + 1;
		current = string.find(delim, previous);
	}
	container.push_back(string.substr(previous, current - previous));
}