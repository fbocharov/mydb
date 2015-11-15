#ifndef Cursor_h
#define Cursor_h

#include <memory>
#include <vector>

#include "DataPage.h"
#include "Record.h"

struct ColumnDescriptor;
struct PageManager;

struct ICursor {
	ICursor(PageManager & pageManager, PageID startPageID,
		std::vector<ColumnDescriptor> const & descriptors);
	virtual ~ICursor() = default;

	Record Get();
	/// Returns true if there is next record and moves to it.
	virtual bool Next();

protected:
	virtual bool AcceptCurrentRecord() = 0;

protected:
	std::vector<ColumnDescriptor> const & m_descriptors;
	PageManager & m_pageManager;
	DataPage m_currentPage;
	size_t m_currentRecordNumber; /// in page
};

#endif // Cursor_h
