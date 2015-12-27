#ifndef Table_h
#define Table_h

#include <map>
#include <string>
#include <memory>

#include <common/Common.h>
#include <common/Condition.h>

#include "DataPage.h"
#include "Index.h"

struct ICursor;
class FullScanCursor;

class Table {
public:
	Table(std::shared_ptr<PageManager> manager, ColumnDescriptors const & descriptors);
	Table(std::shared_ptr<PageManager> manager, ColumnDescriptors const & columnDescriptors, Indices const & indices, PageID firstPage);

	bool AddBTreeIndex(std::string const & name, ColumnDescriptor const & column);
	void Serialize(Page & page) const;
	static Table Deserialize(Page const & page, std::shared_ptr<PageManager> manager);

	ColumnDescriptors const & GetDescription() const;
	bool HasIndex(std::string const & name) const;
	bool Insert(std::vector<std::string> const & columns, Values const & values);

	std::unique_ptr<ICursor> GetCursor(Conditions const & conditions = Conditions());
	std::unique_ptr<FullScanCursor> GetFullScanCursor();

private:
	void AddPage();

private:
	std::shared_ptr<PageManager> m_pageManager;
	ColumnDescriptors m_columnDescriptors;
	Indices m_indices;
	PageID m_firstPageID;
	std::unique_ptr<DataPage> m_pageWithSpace;
};

#endif // Table_h
