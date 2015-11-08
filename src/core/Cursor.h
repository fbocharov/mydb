#ifndef Cursor_h
#define Cursor_h

#include <memory>
#include <vector>

#include <backend/Page.h>

#include "Record.h"

struct ColumnDescriptor;
struct PageManager;

struct ICursor {
	ICursor(std::vector<ColumnDescriptor> const & descriptors,
		PageManager & pageManager, PageID startPageID);
	virtual ~ICursor() = default;

	virtual Record & Get();
	/// Returns true if there is next record and moves current record id to it.
	virtual bool Next();

protected:
	virtual bool AcceptCurrentRecord() = 0;

protected:
	std::vector<ColumnDescriptor> const & m_descriptors;
	PageManager & m_pageManager;
	PageID m_currentPageID;
	size_t m_currentRecordID; /// in page
	Record m_currentRecord;
	size_t m_recordLength;
};

#endif // Cursor_h
