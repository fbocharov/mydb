#include <common/Condition.h>
#include <core/DataPage.h>

#include "BPlusTreeIndexCursor.h"

BPlusTreeIndexCursor::BPlusTreeIndexCursor(ColumnDescriptors const & descriptors, PageManager & manager)
	: CursorImpl(descriptors)
{
}

bool BPlusTreeIndexCursor::Delete() {

}

bool BPlusTreeIndexCursor::HasNext() const {

}

char const * BPlusTreeIndexCursor::GetCurrentRecord() const {
}

void BPlusTreeIndexCursor::GoToNextRecord() {

}
