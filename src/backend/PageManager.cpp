#include <cassert>

#include <utils/Log.h>

#include "Page.h"
#include "PageManager.h"

PageManager::PageManager(std::string const & dbFileName, bool dbFileExist)
	: m_heapFile(dbFileName, dbFileExist)
{}

PageManager::~PageManager() {
	for (auto const & it: m_pageCache)
		FlushPage(it.second);
}

std::weak_ptr<Page> PageManager::AllocatePage() {
	Log(LogType::Info) << "Request for allocate page... ";

	if (MAX_PAGE_CACHE_SIZE == m_pageCache.size())
		PopLRUPage();
	auto pageID = m_heapFile.GetFreePageID();
	auto page = std::make_shared<Page>(pageID);
	m_pageCache[pageID] = page;
	UpdateAccessTime(page);

	Log(LogType::Info) << "done! New page id = " << page->GetID() << std::endl;

	return page;
}

void PageManager::DeallocatePage(PageID id) {
	Log(LogType::Info) << "Deallocating page with id = " << id << "." << std::endl;

	m_heapFile.DeallocatePage(id);
}

std::weak_ptr<Page> PageManager::GetPage(PageID id) {
	Log(LogType::Debug) << "Request for page with id = " << id << std::endl;

	auto it = m_pageCache.find(id);
	if (m_pageCache.end() == it) {
		if (MAX_PAGE_CACHE_SIZE == m_pageCache.size())
			PopLRUPage();
		auto page = std::make_shared<Page>(id);
		m_heapFile.ReadPage(page);
		it = m_pageCache.emplace(id, page).first;
	}

	auto page = it->second;
	UpdateAccessTime(page);

	return page;
}

void PageManager::FlushPage(std::shared_ptr<Page> page) {
	if (page->IsDirty())
		m_heapFile.WritePage(page);
}

void PageManager::PopLRUPage() {
	for (auto const & it: m_pagesOrderByAccessTime)
		if (!it.second->IsPinned()) {
			RemovePage(it.first, it.second);
			return;
		}
	// If all pages are pinned -> just remove lru.
	auto it = m_pagesOrderByAccessTime.begin();
	RemovePage(it->first, it->second);
}

void PageManager::RemovePage(TimePoint const & tp, std::shared_ptr<Page> page) {
	m_pageCache.erase(page->GetID());
	m_pagesOrderByAccessTime.erase(tp);
	m_pagesLastAccessTime.erase(page);
	FlushPage(page);
}

void PageManager::UpdateAccessTime(std::shared_ptr<Page> page) {
	auto const now = std::chrono::system_clock::now();

	auto it = m_pagesLastAccessTime.find(page);
	if (m_pagesLastAccessTime.end() != it) {
		auto lastAccess = it->second;
		m_pagesOrderByAccessTime.erase(lastAccess);
		m_pagesOrderByAccessTime[now] = page;
	}
	// Need to optimize second search?
	m_pagesLastAccessTime[page] = now;
}
