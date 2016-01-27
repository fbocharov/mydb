#ifndef BPlusTreeIndexCursor_h
#define BPlusTreeIndexCursor_h

#include <backend/PageManager.h>
#include <common/Condition.h>

#include "LeafNode.h"
#include <core/CursorImpl.h>

class BPlusTreeIndexCursor : public CursorImpl {
public:
	virtual void MoveToBegin() override;
	virtual bool Delete() override;
	BPlusTreeIndexCursor(PageManager & manager, PageID startPageID, size_t keySize,
		ColumnDescriptors const & descriptors, Condition const & indexCondition);


private:
	virtual char const * GetCurrentRecord() const override;
	virtual void GoToNextRecord() override;
	virtual bool HasNext() const override;

private:
	PageManager & m_pageManager;
	std::unique_ptr<LeafNode::EntryIterator> m_iterator;
	size_t m_indexKeySize;
};

#endif // BPlusTreeIndexCursor_h
