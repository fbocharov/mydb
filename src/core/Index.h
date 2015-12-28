#ifndef Index_h
#define Index_h
#include <string>
#include <vector>

#include <common/Condition.h>
#include <backend/PageManager.h>

#include <memory>

#include "Cursor.h"

class Page;

static size_t constexpr INDEX_NAME_SIZE = 80;
static size_t constexpr INDEX_SIZE = INDEX_NAME_SIZE + COLUMN_NAME_LENGTH + sizeof(uint32_t);

class Index
{
public:
	Index(std::shared_ptr<PageManager> pageManager, std::string const & name, ColumnDescriptor const & column, PageID firstPageID);
	Index(std::shared_ptr<PageManager> pageManager, std::string const & name, ColumnDescriptor const & column);
	
	static Index Deserialize(char const * data, ColumnDescriptors const & columns, std::shared_ptr<PageManager> manager);
	void Serialize(char * data) const;

	bool Insert(Value const & value, PageID pageID, size_t recordNum);
	bool Delete(Value const & value, PageID pageID, size_t recordNum);

	std::unique_ptr<ICursor> GetCursor(Conditions const & conditions = Conditions());

	std::string const & GetName() const;

private:
	std::shared_ptr<PageManager> m_pageManager;
	std::string m_name;
	ColumnDescriptor m_column;
	PageID m_firstPageID;
};

typedef std::map<std::string, Index> Indices;

#endif // Index_h
