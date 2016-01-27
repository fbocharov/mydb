#ifndef FullScanCursor_h
#define FullScanCursor_h

#include <memory>

#include <common/Common.h>

#include "DataPage.h"
#include "CursorImpl.h"

class PageManager;

class FullScanCursor : public CursorImpl {
public:
	FullScanCursor(PageManager & pageManager, PageID startPageID, ColumnDescriptors const & descriptors);

	virtual bool Delete() override;
	void MoveToBegin() override;
private:
	virtual char const * GetCurrentRecord() const override;
	virtual void GoToNextRecord() override;
	virtual bool HasNext() const override;

protected:
	PageManager & m_pageManager;
	std::unique_ptr<DataPage> m_currentPage;
	size_t m_currentRecordNumber; /// in page
	PageID m_firstPageID;
};

#endif // FullScanCursor_h
