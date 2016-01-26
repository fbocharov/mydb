#ifndef BPlusTreeIndexCursor_h
#define BPlusTreeIndexCursor_h

#include <backend/PageManager.h>
#include <common/Condition.h>

#include "LeafNode.h"
#include <core/DeleteCursor.h>

class BPlusTreeIndexCursor : public DeleteCursor {
public:
	BPlusTreeIndexCursor(PageManager & manager, PageID startPageID, size_t keySize,
		ColumnDescriptors const & descriptors, Condition const & indexCondition);

	virtual bool HasNext() const;

private:
	virtual char const * GetCurrentRecord() const override;
	virtual void GoToNextRecord() override;

private:
	PageManager & m_pageManager;
	std::unique_ptr<LeafNode::EntryIterator> m_iterator;
	size_t m_indexKeySize;
};

#endif // BPlusTreeIndexCursor_h
