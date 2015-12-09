#ifndef FullScanCursor_h
#define FullScanCursor_h

#include <memory>

#include <common/Common.h>

#include "Cursor.h"
#include "DataPage.h"

struct PageManager;

class FullScanCursor : public ICursor {
public:
	FullScanCursor(PageManager & pageManager, PageID startPageID, ColumnDescriptors const & descriptors);

	virtual Record Get() override;
	/// Returns true if there is next record and moves to it.
	virtual bool Next() override;

private:
	bool HasNext() const;

protected:
	ColumnDescriptors const & m_descriptors;
	PageManager & m_pageManager;
	std::unique_ptr<DataPage> m_currentPage;
	size_t m_currentRecordNumber; /// in page
};

#endif // FullScanCursor_h

