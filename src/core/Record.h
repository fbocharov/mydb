#ifndef Record_h
#define Record_h

#include <vector>
#include <string>
#include <map>

#include <common/Common.h>

struct DataPage;

class Record {
public:
	Record(DataPage & parentPage, size_t number, char const * data,
		ColumnDescriptors const & descriptors);
	~Record();

	std::vector<std::string> const & GetValues() const;

//	std::string & operator[] (std::string const & key);
//	std::string const & operator[] (std::string const & key) const;

private:
	DataPage & m_parentPage;
	size_t const m_number;
	std::map<std::string, std::string *> m_columnValue;
	std::vector<std::string> m_values;
	bool m_isDirty;
};

#endif // Record_h
