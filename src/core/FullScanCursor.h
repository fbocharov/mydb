#ifndef FullScanCursor_h
#define FullScanCursor_h

#include <memory>

#include <common/Common.h>
#include <common/Value.h>
#include <common/Condition.h>

#include "Cursor.h"
#include "DataPage.h"

struct PageManager;

class FullScanCursor : public ICursor {
public:
	FullScanCursor(PageManager & pageManager, PageID startPageID, ColumnDescriptors const & descriptors,
		Conditions const & conditions = Conditions());

	virtual Value Get(std::string const & column) const override;
	virtual Values GetAll() const override;
	/// Returns true if there is next record and moves to it.
	virtual bool Next() override;
	virtual bool Update(std::map<std::string, Value> const & colVals) override;
	virtual bool Delete() override;

private:
	bool HasNext() const;
	bool SatisfiesAll() const;

protected:
	ColumnDescriptors const & m_descriptors;
	PageManager & m_pageManager;
	std::unique_ptr<DataPage> m_currentPage;
	size_t m_currentRecordNumber; /// in page
	Conditions m_conditions;
};

#endif // FullScanCursor_h
