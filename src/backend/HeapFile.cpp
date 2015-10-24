#include <cassert>

#include <utils/Utils.h>

#include "HeapFile.h"
#include "IOException.h"

HeapFile::HeapFile(std::string const & filename, bool existent)
	: m_file(filename, std::ios_base::in | std::ios_base::out | std::ios_base::binary)
	, m_freePagesListHead(INVALID_PAGE_ID)
	, m_maxPageID(0)
{
	if (m_file.fail())
		throw IOException("Can't open file " + filename + ".");
	m_file.exceptions(std::ios_base::eofbit | std::ios_base::failbit | std::ios_base::badbit);
	m_file.rdbuf()->pubsetbuf(nullptr, 0); // turn off buffering for writing.

	if (existent)
		ReadHeader();
}

HeapFile::~HeapFile() {
	WriteHeader();
}

PageID HeapFile::GetFreePageID() {
	if (INVALID_PAGE_ID == m_freePagesListHead)
		ReservePages();

	return FreePagesStackPop();
}

void HeapFile::DeallocatePage(PageID id) {
	FreePagesStackPush(id);
}


void HeapFile::ReadPage(std::shared_ptr<Page> page) {
	auto const pageID = page->GetID();
	size_t const offset = HEADER_SIZE + pageID * Page::PAGE_SIZE;
	try {
		m_file.seekg(offset, std::ios_base::beg);
		m_file.read(page->GetData(), Page::PAGE_SIZE);
	} catch (std::ifstream::failure const & e) {
		throw IOException(e.what());
	}
}

void HeapFile::WritePage(std::shared_ptr<Page> page) {
	auto const pageID = page->GetID();
	size_t const offset = HEADER_SIZE + pageID * Page::PAGE_SIZE;
	try {
		m_file.seekp(offset, std::ios_base::beg);
		m_file.write(page->GetData(), Page::PAGE_SIZE);
	} catch (std::ofstream::failure const & e) {
		throw IOException(e.what());
	}
}

void HeapFile::ReadHeader() {
	try {
		char buf[HEADER_SIZE] = {0};
		// Assume pos in file = 0.
		m_file.read(buf, HEADER_SIZE);
		size_t m = 0;
		BytesToNumber(buf, m);
		if (MAGIC != m) {
			; // Should throw?
		}
		BytesToNumber(buf + sizeof(MAGIC), m_maxPageID);
		BytesToNumber(buf + sizeof(MAGIC) + sizeof(m_maxPageID), m_freePagesListHead);
	} catch (std::ifstream::failure const & e) {
		throw IOException(e.what());
	}
}

void HeapFile::WriteHeader() {
	char buf[HEADER_SIZE] = {0};
	NumberToBytes(MAGIC, buf);
	NumberToBytes(m_maxPageID, buf + sizeof(MAGIC));
	NumberToBytes(m_freePagesListHead, buf + sizeof(MAGIC) + sizeof(m_maxPageID));
	try {
		m_file.seekp(0, std::ios_base::beg);
		m_file.read(buf, HEADER_SIZE);
	} catch (std::ifstream::failure const & e) {
		throw IOException(e.what());
	}
}

void HeapFile::ReservePages() {
	char nulls[Page::PAGE_SIZE] = {0};
	m_file.seekp(0, std::ios_base::end);
	for (size_t i = 0; i < RESERVE_PAGE_COUNT; ++i) {
		m_file.write(nulls, Page::PAGE_SIZE);
		FreePagesStackPush(m_maxPageID);
		++m_maxPageID;
	}
}

PageID HeapFile::FreePagesStackPop() {
	assert(INVALID_PAGE_ID != m_freePagesListHead);

	PageID const pageID = m_freePagesListHead;
	size_t const offset = HEADER_SIZE + (m_freePagesListHead + 1) * Page::PAGE_SIZE - sizeof(PageID);
	char buf[sizeof(PageID)] = {0};
	try {
		m_file.seekp(offset, std::ios_base::beg);
		m_file.read(buf, sizeof(PageID));
	} catch (std::ofstream::failure const & e) {
		throw IOException(e.what());
	}
	BytesToNumber(buf, m_freePagesListHead);
	return pageID;
}

void HeapFile::FreePagesStackPush(PageID id) {
	char buf[sizeof(PageID)] = {0};
	NumberToBytes(m_freePagesListHead, buf);
	size_t const offset = HEADER_SIZE + (id + 1) * Page::PAGE_SIZE - sizeof(PageID);
	try {
		m_file.seekp(offset, std::ios_base::beg);
		m_file.write(buf, sizeof(PageID));
	} catch (std::ofstream::failure const & e) {
		throw IOException(e.what());
	}
	m_freePagesListHead = id;
}
