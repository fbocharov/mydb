#ifndef DataPage_h
#define DataPage_h

#include <vector>

#include <backend/Page.h>
#include <backend/PageManager.h>

#include "Common.h"
#include "Record.h"

using std::uint16_t;

class DataPage {
public:
	DataPage(PageManager & manager, PageID pageID,
		std::vector<ColumnDescriptor> const & descrpitors);
	~DataPage();

	bool AppendRecord(Record const & record);
	void UpdateRecord(size_t number, std::string const & column, std::string const & value);
	void DeleteRecord(size_t number);

	Record GetRecord(size_t number) const;
	uint16_t GetRecordCount() const;
	PageID GetNextPageID() const;
	PageID GetPrevPageID() const;
	bool HasFreeSpace() const;

private:
	uint16_t CalculateRecordOffset(size_t recordNumber) const;
	void ReadHeader(char const * data);
	void WriteHeader(char * data);
	std::shared_ptr<Page> GetNativePage(bool needDirty = false) const;

private:
	PageManager * m_pageManager;
	PageID m_currentPageID;
	std::vector<ColumnDescriptor> const * m_columnDescriptors;
	// Caching them so it would not cause page fault when calling getters.
	PageID m_prevPageID;
	PageID m_nextPageID;
	uint16_t m_recordCount;
	uint16_t m_freeSpaceOffset;

	uint16_t m_recordLength;
	static uint16_t constexpr HEADER_SIZE =
		2 * sizeof(uint16_t); // record count + free space offset
};

#endif // DataPage_h
