#ifndef BPlusTreeIndexCursor_h
#define BPlusTreeIndexCursor_h

#include <backend/PageManager.h>
#include <common/Condition.h>
#include <core/CursorImpl.h>
#include <core/DataPage.h>

#include "LeafNode.hpp"

template<typename KeyT>
class BPlusTreeIndexCursor : public CursorImpl {
public:
	BPlusTreeIndexCursor(ColumnDescriptors const & descriptors, PageManager & manager,
		Condition const & from, Condition const & to, PageID startPage, size_t firstRecord);

	virtual PageID GetCurrentPage() const override;
	virtual std::uint32_t GetCurrentRecordNumber() const override;
	virtual bool Delete() override;
	virtual void MoveToBegin() override;

private:
	virtual bool HasNext() const override;
	virtual char const * GetCurrentRecord() const override;
	virtual void GoToNextRecord() override;

private:
	char * GetPageBytes(bool needDirty = false) const;

private:
	PageManager & m_pageManager;
	Condition m_from;
	Condition m_to;
	PageID m_currentPage;
	size_t m_currentEntry;

	PageID m_startPage;
	size_t m_startEntry;
};


template<typename KeyT>
BPlusTreeIndexCursor<KeyT>::BPlusTreeIndexCursor(ColumnDescriptors const & descriptors, PageManager & manager,
		Condition const & from, Condition const & to, PageID startPage, size_t startEntry)
	: CursorImpl(descriptors)
	, m_pageManager(manager)
	, m_from(from)
	, m_to(to)
	, m_currentPage(startPage)
	, m_currentEntry(startEntry)
	, m_startPage(startPage)
	, m_startEntry(startEntry)
{}

template<typename KeyT>
PageID BPlusTreeIndexCursor<KeyT>::GetCurrentPage() const {
	LeafNode<KeyT> leaf(GetPageBytes());
	return leaf.GetRecordPage(m_currentEntry);
}

template<typename KeyT>
std::uint32_t BPlusTreeIndexCursor<KeyT>::GetCurrentRecordNumber() const {
	LeafNode<KeyT> leaf(GetPageBytes());
	return leaf.GetRecordNumber(m_currentEntry);
}


template<typename KeyT>
bool BPlusTreeIndexCursor<KeyT>::Delete() {
	LeafNode<KeyT> leaf(GetPageBytes());
	PageID recordPage = leaf.GetRecordPage(m_currentEntry);
	size_t recordNumber = leaf.GetRecordNumber(m_currentEntry);

	DataPage page(m_pageManager, recordPage, m_descriptors);
	leaf.MarkDeleted(m_currentEntry);
	return page.DeleteRecord(recordNumber);
}

template<typename KeyT>
void BPlusTreeIndexCursor<KeyT>::MoveToBegin() {
	m_currentPage = m_startPage;
	m_currentEntry = m_startEntry;
}


template<typename KeyT>
bool BPlusTreeIndexCursor<KeyT>::HasNext() const {
	LeafNode<KeyT> leaf(GetPageBytes());

	if (m_currentEntry + 1 < LeafNode<KeyT>(GetPageBytes()).GetEntryCount()) {
		Value value(ValueType::UNKNOWN, leaf.GetKey(m_currentEntry + 1));
		return m_from.Satisfies(value) && m_to.Satisfies(value);
	}

	return INVALID_PAGE_ID != m_pageManager.GetPage(m_currentPage).lock()->GetNextPageID();
}

template<typename KeyT>
char const * BPlusTreeIndexCursor<KeyT>::GetCurrentRecord() const {
	LeafNode<KeyT> leaf(GetPageBytes());
	PageID recordPage = leaf.GetRecordPage(m_currentEntry);
	size_t recordNumber = leaf.GetRecordNumber(m_currentEntry);

	DataPage page(m_pageManager, recordPage, m_descriptors);
	if (page.RecordIsDeleted(recordNumber))
		leaf.MarkDeleted(m_currentEntry);
	return page.GetRawRecord(recordNumber);
}

template<typename KeyT>
void BPlusTreeIndexCursor<KeyT>::GoToNextRecord() {
	LeafNode<KeyT> leaf(GetPageBytes());
	if (m_currentEntry + 1 == leaf.GetEntryCount()) {
		m_currentPage = m_pageManager.GetPage(m_currentPage).lock()->GetNextPageID();
		m_currentEntry = 0;
	} else
		++m_currentEntry;
}

template<typename KeyT>
char * BPlusTreeIndexCursor<KeyT>::GetPageBytes(bool needDirty) const {
	auto page = m_pageManager.GetPage(m_currentPage).lock();
	if (needDirty)
		page->SetDirty();
	return page->GetData();
}

#endif // BPlusTreeIndexCursor_h
