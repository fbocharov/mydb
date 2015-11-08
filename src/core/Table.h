#ifndef Table_h
#define Table_h

#include <vector>

#include "Common.h"

class Table {
public:
	Table();
	~Table();

	void Insert();
	void Delete(); // accepts iterator on deleting object

	void GetIterator();

private:
	std::vector<ColumnDescriptor> m_columnDescriptors;
};

#endif // Table_h
