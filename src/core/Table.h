#ifndef Table_h
#define Table_h

#include <map>
#include <string>
#include <memory>

#include "DataPage.h"
#include "Common.h"

class ICursor;

class Table {
public:
	Table(PageManager & manager, ColumnDescriptors const & descriptors);
	Table(ColumnDescriptors const & columnDescriptors, PageManager & pageManager, PageID firstPage);

	static std::unique_ptr<Table> Deserialize(Page const & page, PageManager & manager);
	void Serialize(Page & page);

	void Insert(std::map<std::string, std::string> const & colVals);
	std::unique_ptr<ICursor> GetCursor();

private:
	void AddPage();

private:
	ColumnDescriptors m_columnDescriptors;
	PageManager & m_pageManager;
	PageID m_firstPageID;
	std::unique_ptr<DataPage> m_pageWithSpace;
};

#endif // Table_h
