#ifndef Record_h
#define Record_h

#include <vector>
#include <string>

#include <common/Common.h>

class Record {
public:
	Record() = default;
	Record(char const * data, ColumnDescriptors const & descriptors);

	Values const & GetValues() const;
	Value const & operator[] (std::string const & column) const;
	bool IsDeleted() const;

private:
	ColumnDescriptors m_descriptors;
	Values m_values;
	bool m_isDeleted;
};

#endif // Record_h
