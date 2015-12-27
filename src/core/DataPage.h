#ifndef DataPage_h
#define DataPage_h

#include <map>
#include <cstdint>
#include <memory>

#include <backend/Page.h>
#include <common/Common.h>
#include <common/Value.h>

struct PageManager;

class DataPage {
public:
	DataPage(PageManager & manager, PageID pageID, ColumnDescriptors const & descrpitors);
	~DataPage();

	bool AppendRecord(std::map<std::string, Value> const & colVals);
	bool UpdateRecord(size_t number, const std::map<std::string, Value> &colVals);
	bool DeleteRecord(size_t number);
	char const * GetRawRecord(size_t number) const;

	size_t GetRecordCount() const;
	PageID GetID() const;
	PageID GetNextPageID() const;
	PageID GetPrevPageID() const;
	bool HasFreeSpace() const;

private:
	size_t CalculateRecordOffset(size_t number) const;
	void ReadHeader(char const * data);
	void WriteHeader(char * data);
	std::shared_ptr<Page> GetNativePage(bool needDirty = false) const;
	ColumnDescriptor const & FindDescriptor(std::string const & name);

private:
	PageManager & m_pageManager;
	PageID m_id;
	ColumnDescriptors m_columnDescriptors;
	std::map<std::string, size_t> m_columnOffsets;
	// Caching them so it would not cause page fault when calling getters.
	PageID m_prevPageID;
	PageID m_nextPageID;
	uint32_t m_recordCount;
	uint32_t m_freeSpaceOffset;

	size_t m_recordLength;
	static uint32_t constexpr HEADER_SIZE =
		2 * sizeof(uint32_t); /// record count + free space offset
};

#endif // DataPage_h
