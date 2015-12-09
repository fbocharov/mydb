#include <cassert>
#include <experimental/filesystem>

#include <utils/Utils.h>

#include "HeapFile.h"
#include "IOException.h"

HeapFile::HeapFile(std::string const & filename, bool existent)
	: m_filename(filename)
	, m_freePagesListHead(INVALID_PAGE_ID)
	, m_maxPageID(1) // NOTE: 0 page is system page.
{
	auto flags = std::ios_base::in | std::ios_base::out | std::ios_base::binary;
	if (!existent)
		flags |= std::ios_base::trunc;
	m_file = std::fstream(filename, flags);
	if (m_file.fail())
		throw IOException("Can't open file " + filename + ".");
	m_file.exceptions(std::ios_base::eofbit | std::ios_base::failbit | std::ios_base::badbit);
	m_file.rdbuf()->pubsetbuf(nullptr, 0); // turn off buffering for writing.

	if (existent)
		ReadHeader();
	else {
		WriteHeader();
		Expand(Page::PAGE_SIZE); // Reserving space for system page.
	}
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
	uint64_t const offset = CalculatePageOffset(pageID);
	try {
		m_file.seekg(offset, std::ios_base::beg);
		m_file.read(page->m_data, Page::PAGE_SIZE);
	} catch (std::ifstream::failure const & e) {
		throw IOException(e.what());
	}
}

void HeapFile::WritePage(std::shared_ptr<Page> page) {
	auto const pageID = page->GetID();
	uint64_t const offset = CalculatePageOffset(pageID);
	try {
		m_file.seekp(offset, std::ios_base::beg);
		m_file.write(page->m_data, Page::PAGE_SIZE);
		m_file.flush();
	} catch (std::ofstream::failure const & e) {
		throw IOException(e.what());
	}
}

void HeapFile::ReadHeader() {
	try {
		char buf[HEADER_SIZE] {};
		m_file.seekg(0, std::ios_base::beg);
		m_file.read(buf, HEADER_SIZE);
		uint16_t m = 0;
		BytesToNumber(buf, m);
		if (MAGIC != m)
			assert(false && "Magic don't match"); // Should throw?
		BytesToNumber(buf + sizeof(MAGIC), m_maxPageID);
		BytesToNumber(buf + sizeof(MAGIC) + sizeof(m_maxPageID), m_freePagesListHead);
	} catch (std::ifstream::failure const & e) {
		throw IOException(e.what());
	}
}

void HeapFile::WriteHeader() {
	char buf[HEADER_SIZE] {};
	NumberToBytes(MAGIC, buf);
	NumberToBytes(m_maxPageID, buf + sizeof(MAGIC));
	NumberToBytes(m_freePagesListHead, buf + sizeof(MAGIC) + sizeof(m_maxPageID));
	try {
		m_file.seekp(0, std::ios_base::beg);
		m_file.write(buf, HEADER_SIZE);
		m_file.flush();
	} catch (std::ifstream::failure const & e) {
		throw IOException(e.what());
	}
}

void HeapFile::ReservePages() {
	Expand(RESERVE_PAGE_COUNT * Page::PAGE_SIZE);
	for (size_t i = 0; i < RESERVE_PAGE_COUNT; ++i)
		FreePagesStackPush(m_maxPageID++);
}

void HeapFile::Expand(uint64_t size) {
	auto oldSize = std::experimental::filesystem::file_size(m_filename);
	std::experimental::filesystem::resize_file(m_filename, oldSize + size);
}


PageID HeapFile::FreePagesStackPop() {
	assert(INVALID_PAGE_ID != m_freePagesListHead);

	PageID const pageID = m_freePagesListHead;
	uint64_t const offset = CalculatePageOffset(m_freePagesListHead) + Page::PAGE_SIZE - sizeof(PageID);
	char buf[sizeof(PageID)] {};
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
	char buf[sizeof(PageID)] {};
	NumberToBytes(m_freePagesListHead, buf);
	uint64_t const offset = CalculatePageOffset(id) + Page::PAGE_SIZE - sizeof(PageID);
	try {
		m_file.seekp(offset, std::ios_base::beg);
		m_file.write(buf, sizeof(PageID));
	} catch (std::ofstream::failure const & e) {
		throw IOException(e.what());
	}
	m_freePagesListHead = id;
}

uint64_t HeapFile::CalculatePageOffset(PageID pageID) const {
	uint64_t offset = HEADER_SIZE + Page::PAGE_SIZE * pageID;
	if (std::experimental::filesystem::file_size(m_filename) <= offset)
		throw IOException("Trying to access page outside file. Page id = " + std::to_string(pageID));

	return offset;
}
