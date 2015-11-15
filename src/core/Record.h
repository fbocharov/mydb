#ifndef Record_h
#define Record_h

#include <vector>
#include <string>
#include <map>

using std::uint16_t;

struct ColumnDescriptor;
struct DataPage;

class Record {
public:
	Record(DataPage & parentPage, uint16_t number, char const * data,
		std::map<std::string, ColumnDescriptor> const & descriptors);
	~Record();

	std::string & operator[] (std::string const & key);
	std::string const & operator[] (std::string const & key) const;

private:
	DataPage & m_parentPage;
	uint16_t const m_number;
	std::map<std::string, ColumnDescriptor> const & m_descriptors;
	std::map<std::string, std::string> m_columnValue;
};

#endif // Record_h
