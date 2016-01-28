#ifndef Utils_h
#define Utils_h

#include <cstddef>
#include <cstring>
#include <vector>
#include <string>

// Playing with fire here!

template<typename T>
inline void BytesToNumber(char const * bytes, T & number) {
	number = *(reinterpret_cast<T const *>(bytes));
}

template<typename T>
inline void NumberToBytes(T number, char * bytes) {
	char const * numberPtr = reinterpret_cast<char const *>(&number);
	::memcpy(bytes, numberPtr, sizeof(T));
}

inline std::pair<std::string, std::string> SplitQualified(std::string const & value)
{
	auto dotPost = value.find('.');
	return dotPost == value.npos
		? std::make_pair("", value)
		: std::make_pair(value.substr(0, dotPost), value.substr(dotPost + 1));
}

inline std::vector<std::pair<std::string, std::string>> SplitQualifiedVector(std::vector<std::string> const & source) {
	std::vector<std::pair<std::string, std::string>> result;
	for(auto const & line : source) {
		result.push_back(SplitQualified(line));
	}

	return result;
}

inline std::pair<std::vector<std::string>, std::vector<std::string>> SplitQualifiedVectorUnzip(std::vector<std::string> const & source)
{
	std::pair<std::vector<std::string>, std::vector<std::string>> result;
	for (auto const splitted : SplitQualifiedVector(source))
	{
		result.first.push_back(splitted.first);
		result.second.push_back(splitted.second);
	}

	return result;
}

template<typename T>
T GetValueByOffset(char const * bytes, size_t offset) {
	T value;
	bytes += offset;
	BytesToNumber(bytes, value);
	return value;
}

#endif // Utils_h
