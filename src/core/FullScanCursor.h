#ifndef FullScanCursor_h
#define FullScanCursor_h

#include <memory>

#include <common/Common.h>
#include <common/Value.h>
#include <common/Condition.h>

#include "Cursor.h"
#include "DataPage.h"

struct PageManager;

class FullScanCursor : public Cursor {
public:
	FullScanCursor(PageManager & pageManager, PageID startPageID, ColumnDescriptors const & descriptors,
		Conditions const & conditions = Conditions());

	virtual bool HasNext() const override;

	virtual bool Update(std::map<std::string, Value> const & colVals) override;
	virtual bool Delete() override;

private:
	virtual char const * GetCurrentRecord() const override;
	virtual void GoToNextRecord() override;

protected:
	PageManager & m_pageManager;
	std::unique_ptr<DataPage> m_currentPage;
	size_t m_currentRecordNumber; /// in page
};

#endif // FullScanCursor_h
