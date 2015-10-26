#ifndef IOException_h
#define IOException_h

#include <stdexcept>

class IOException : std::logic_error {
public:
	IOException(std::string const & msg)
		: std::logic_error(msg)
	{}
};

#endif // IOException_h
