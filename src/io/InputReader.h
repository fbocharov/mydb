#ifndef InputReader_h
#define InputReader_h

#include <istream>
#include <string>

class InputReader {
public:
	InputReader(std::istream & stream);

	std::string Read();

private:
	std::istream & m_stream;
};

#endif // InputReader_h
