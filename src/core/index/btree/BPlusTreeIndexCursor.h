#ifndef BPlusTreeIndexCursor_h
#define BPlusTreeIndexCursor_h

#include <backend/PageManager.h>
#include <common/Condition.h>
#include <core/CursorImpl.h>

class BPlusTreeIndexCursor : public CursorImpl {
public:
	BPlusTreeIndexCursor(const ColumnDescriptors &descriptors, PageManager & manager);

	virtual bool Delete() override;

private:
	virtual bool HasNext() const override;
	virtual char const * GetCurrentRecord() const override;
	virtual void GoToNextRecord() override;
};

#endif // BPlusTreeIndexCursor_h
