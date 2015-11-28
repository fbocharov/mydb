#ifndef FullScanCursor_h
#define FullScanCursor_h

#include "Cursor.h"

class FullScanCursor : public ICursor {
public:
	FullScanCursor(PageManager & pageManager, PageID startPageID,
			ColumnDescriptors const & descriptors)
		: ICursor(pageManager, startPageID, descriptors)
	{}

protected:
	virtual bool AcceptCurrentRecord() override {
		// When doing fullscan we should accept every record.
		return true;
	}
};

#endif // FullScanCursor_h

