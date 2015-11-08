#include <cstring>

#include <utils/Utils.h>

#include "Page.h"

Page::Page(PageID id, char const * data)
	: m_data{}
	, m_id(id)
	, m_isDirty(false)
	, m_isPinned(false)
{
	if (data)
		::memcpy(m_data, data, PAGE_SIZE);
}

char const * Page::GetData() const {
	return m_data + sizeof(PageID);
}

char * Page::GetData() {
	return m_data + sizeof(PageID);
}

bool Page::IsDirty() const {
	return m_isDirty;
}

void Page::SetDirty() {
	m_isDirty = true;
}

void Page::Pin() {
	m_isPinned = true;
}

void Page::Unpin() {
	m_isPinned = false;
}

bool Page::IsPinned() const {
	return m_isPinned;
}

PageID Page::GetID() const {
	return m_id;
}

PageID Page::GetPrevPageID() const {
	PageID prevID = 0;
	BytesToNumber(m_data, prevID);
	return prevID;
}

PageID Page::GetNextPageID() const {
	PageID nextID = 0;
	BytesToNumber(m_data + PAGE_SIZE - sizeof(PageID), nextID);
	return nextID;
}
