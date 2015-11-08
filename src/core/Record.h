#ifndef Record_h
#define Record_h

#include <vector>
#include <string>

struct ColumnDescriptor;

class Record {
public:
	Record(std::vector<ColumnDescriptor> const & descriptors, char * data)
		: m_descriptors(&descriptors)
		, m_data(data)
	{}

	std::string & operator[] (std::string const & key);

private:
	std::vector<ColumnDescriptor> const * m_descriptors;
	char * m_data;
};

#endif // Record_h
