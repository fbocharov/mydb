#ifndef Table_h
#define Table_h

#include <map>
#include <string>
#include <memory>

#include <common/Common.h>

#include "DataPage.h"

class ICursor;

class Table {
public:
	Table(PageManager & manager, ColumnDescriptors const & descriptors);
	Table(ColumnDescriptors const & columnDescriptors, PageManager & pageManager, PageID firstPage);

	static std::unique_ptr<Table> Deserialize(Page const & page, PageManager & manager);
	void Serialize(Page & page);

	ColumnDescriptors const & GetDescription() const;
	bool Insert(std::vector<std::string> const & columns, std::vector<std::string> const & values);
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
