#ifndef Index_h
#define Index_h
#include <string>
#include <vector>

#include <memory>
#include "Cursor.h"
#include <common/Condition.h>
#include <Page.h>

class Page;
class PageManager;

static size_t constexpr INDEX_NAME_SIZE = 80;
static size_t constexpr INDEX_SIZE = INDEX_NAME_SIZE + COLUMN_NAME_LENGTH + sizeof(uint32_t);

class Index
{
public:
	Index(PageManager & pageManager, std::string const & name, ColumnDescriptor const & column, PageID firstPageID);
	Index(PageManager & pageManager, std::string const & name, ColumnDescriptor const & column);
	
	static std::unique_ptr<Index> Deserialize(char const * data, ColumnDescriptors const & columns, PageManager & manager);
	void Serialize(char * data) const;

	bool Insert(Value const& value, PageID pageID, size_t recordNum);

	std::unique_ptr<ICursor> GetCursor(Conditions const & conditions = Conditions());

	std::string const & GetName() const;

private:
	PageManager & m_pageManager;
	std::string m_name;
	ColumnDescriptor m_column;
	PageID m_firstPageID;
};

typedef std::map<std::string, std::unique_ptr<Index>> Indices;

#endif // Index_h
