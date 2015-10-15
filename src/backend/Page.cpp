#include "Page.h"

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

size_t Page::GetID() const {
	return m_id;
}
