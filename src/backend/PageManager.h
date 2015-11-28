#ifndef PageManager_h
#define PageManager_h

#include <map>
#include <memory>
#include <chrono>
#include <cstdint>

#include "HeapFile.h"

class Page;

class PageManager {
public:
	PageManager(std::string const & dbFileName, bool dbFileExist);
	~PageManager();

	std::weak_ptr<Page> AllocatePage();
	void DeallocatePage(PageID id);
	std::weak_ptr<Page> GetPage(PageID id);

	bool PageInCache(PageID id) const;

private:
	typedef std::chrono::system_clock::time_point TimePoint;

private:
	void FlushPage(std::shared_ptr<Page> page);
	void PopLRUPage();
	void RemovePage(TimePoint const & tp, std::shared_ptr<Page> page);
	void UpdateAccessTime(std::shared_ptr<Page> page);

private:
	std::map<PageID, std::shared_ptr<Page>> m_pageCache;
	HeapFile m_heapFile;
	std::map<TimePoint, std::shared_ptr<Page>> m_pagesOrderByAccessTime;
	std::map<std::shared_ptr<Page>, TimePoint> m_pagesLastAccessTime;

	static size_t constexpr MAX_PAGE_CACHE_SIZE = 128;
};

#endif // PageManager_h
