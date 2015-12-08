#include <string>

#include "InputReader.h"

InputReader::InputReader(std::istream & stream)
	: m_stream(stream)
{}

std::string InputReader::Read() {
	std::string input;
	std::string tmp;
	while (true) {
		std::getline(m_stream, tmp);
		if (tmp.empty())
			break;
		input += tmp;
	}
	return input;
}
