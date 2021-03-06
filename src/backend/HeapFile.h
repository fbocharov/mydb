#ifndef HeapFile_h
#define HeapFile_h

#include <fstream>
#include <memory>
#include <cstdint>

using std::uint64_t;

#include "Page.h"

class HeapFile {
public:
	explicit HeapFile(std::string const & filename, bool existent);
	~HeapFile();

	PageID GetFreePageID();
	void DeallocatePage(PageID id);
	void ReadPage(std::shared_ptr<Page> page);
	void WritePage(std::shared_ptr<Page> page);

private:
	void ReadHeader();
	void WriteHeader();
	void ReservePages();
	void Expand(uint64_t size);

	PageID FreePagesStackPop();
	void FreePagesStackPush(PageID id);

	uint64_t CalculatePageOffset(PageID pageID) const;

private:
	std::string m_filename;
	std::fstream m_file;
	PageID m_freePagesListHead;
	PageID m_maxPageID;

	static size_t constexpr RESERVE_PAGE_COUNT = 10;
	static uint16_t constexpr MAGIC = 0b0101010101010101;
	static uint16_t constexpr HEADER_SIZE = sizeof(MAGIC) + sizeof(PageID) + sizeof(PageID);
};

#endif // HeapFile_h
