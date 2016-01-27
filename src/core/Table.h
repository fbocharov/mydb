#ifndef Table_h
#define Table_h

#include <map>
#include <memory>

#include <common/Common.h>
#include <common/Condition.h>

#include "DataPage.h"

class Cursor;
class DeleteCursor;

enum CursorType {
	FULL_SCAN,
	INDEX
};

class Table {
public:
	Table(std::shared_ptr<PageManager> manager, ColumnDescriptors const & descriptors);
	Table(std::shared_ptr<PageManager> manager, ColumnDescriptors const & descriptors, PageID firstPage);

	void Serialize(Page & page) const;
	static Table Deserialize(Page const & page, std::shared_ptr<PageManager> manager);

	ColumnDescriptors const & GetDescription() const;
	bool HasIndex(std::string const & column) const;

	bool Insert(std::vector<std::string> const & columns, Values const & values);
	std::unique_ptr<DeleteCursor> GetDeleteCursorByType(CursorType type, Conditions const & conditions = Conditions());
	std::unique_ptr<Cursor> GetCursorByType(CursorType type, Conditions const& conditions = Conditions()) const;

private:
	void AddPage();

private:
	std::shared_ptr<PageManager> m_pageManager;
	ColumnDescriptors m_columnDescriptors;
	PageID m_firstPageID;
	std::unique_ptr<DataPage> m_pageWithSpace;
};

#endif // Table_h
