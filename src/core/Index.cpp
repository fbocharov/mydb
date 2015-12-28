#include "Index.h"
#include "IndexCursor.h"

Index::Index(std::shared_ptr<PageManager> pageManager, std::string const & name, ColumnDescriptor const & column, PageID firstPageID)
	: m_pageManager(pageManager)
	, m_name(name)
	, m_column(column)
	, m_firstPageID(firstPageID)
{}

Index::Index(std::shared_ptr<PageManager> pageManager, std::string const& name, ColumnDescriptor const& column)
	: m_pageManager(pageManager)
	, m_name(name)
	, m_column(column)
	, m_firstPageID(0)
{
	auto firstPage = m_pageManager->AllocatePage().lock();
	m_firstPageID = firstPage->GetID();
	firstPage->SetNextPageID(INVALID_PAGE_ID);
	firstPage->SetPrevPageID(m_firstPageID);
	firstPage->SetDirty();
}

Index Index::Deserialize(char const * data, ColumnDescriptors const & columns, std::shared_ptr<PageManager> manager)
{
	char name[INDEX_NAME_SIZE + 1] = {0};
	memcpy(name, data, INDEX_NAME_SIZE);
	data += INDEX_NAME_SIZE;

	char columnName[COLUMN_NAME_LENGTH + 1] = {0};
	memcpy(columnName, data, COLUMN_NAME_LENGTH);
	data += COLUMN_NAME_LENGTH;

	uint32_t firstPageID = 0;
	BytesToNumber(data, firstPageID);

	auto const & col = GetDescriptorByName(columns, columnName);

	return Index(manager, name, col, firstPageID);
}

void Index::Serialize(char* data) const
{
	memcpy(data, m_name.c_str(), m_name.size() + 1);
	data += INDEX_NAME_SIZE;
	memcpy(data, m_column.name, COLUMN_NAME_LENGTH);
	data += COLUMN_NAME_LENGTH;
	NumberToBytes(m_firstPageID, data);
}

bool Index::Insert(Value const& value, PageID pageID, size_t recordNum)
{
	return true;
}

bool Index::Delete(Value const& value, PageID pageID, size_t recordNum)
{
	return true;
}

std::unique_ptr<ICursor> Index::GetCursor(Conditions const& conditions)
{
	return std::make_unique<IndexCursor>();
}

std::string const& Index::GetName() const {
	return m_name;
}
