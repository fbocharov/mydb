#include <string>

#include "InputReader.h"

InputReader::InputReader(std::istream & stream)
	: m_stream(stream)
{}

std::string InputReader::Read() {
	std::string input;
	std::string tmp;
	size_t endlCount = 0;
	while (endlCount != 1) {
		std::getline(m_stream, tmp);
		if (tmp == "")
			++endlCount;
		else
			input += tmp;
	}
	return input;
}
