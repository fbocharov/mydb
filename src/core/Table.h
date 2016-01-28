#ifndef Table_h
#define Table_h

#include <map>
#include <memory>

#include <common/Common.h>
#include <common/Condition.h>

#include "DataPage.h"
#include "index/Index.h"
#include "index/btree/BPlusTreeIndex.hpp"

class InternalCursor;

enum class CursorType {
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
	bool CreateIndex(std::string const & column, std::string const & name, IndexType type, bool isUnique);
	std::unique_ptr<InternalCursor> GetCursorByType(CursorType type, Condition condition = Condition());

private:
	void AddPage();
	ColumnDescriptor & FindDescriptor(std::string const & name);
	void FillIndex(std::string const & column, std::shared_ptr<Index> & index);
	std::shared_ptr<Index> FindIndex(std::string const & column) const;
	void UpdateIndices(std::map<std::string, Value> const & colVals, PageID pageID, std::uint32_t recordNumber);
	char *SerializeIndices(char * bytes) const;
	void DeserializeIndices(char const * bytes);

private:
	std::shared_ptr<PageManager> m_pageManager;
	ColumnDescriptors m_columnDescriptors;
	PageID m_firstPageID;
	std::unique_ptr<DataPage> m_pageWithSpace;
	std::map<std::string, std::string> m_columnIndexName;
	std::map<std::string, std::shared_ptr<Index>> m_indices;
};

#endif // Table_h
