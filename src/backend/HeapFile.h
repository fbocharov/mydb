#ifndef HeapFile_h
#define HeapFile_h

#include <fstream>
#include <memory>
#include <string>

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

	PageID FreePagesStackPop();
	void FreePagesStackPush(PageID id);

private:
	std::fstream m_file;
	PageID m_freePagesListHead;
	PageID m_maxPageID;

	static size_t constexpr RESERVE_PAGE_COUNT = 100;
	static std::uint16_t constexpr MAGIC = 0b0101010101010101;
	static size_t constexpr HEADER_SIZE =
			sizeof(MAGIC) + sizeof(m_maxPageID) +
			sizeof(m_freePagesListHead);
};

#endif // HeapFile_h