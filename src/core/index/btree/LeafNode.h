#ifndef LeafNode_h
#define LeafNode_h

#include <backend/PageManager.h>
#include <common/Value.h>
#include <utils/Utils.h>

#include "Node.h"

class LeafNode : public Node {
public:
	class EntryIterator {
	public:
		EntryIterator(PageManager & manager, PageID pageID, std::uint32_t entryCount, size_t keySize);

		EntryIterator & operator++ ();
		bool IsValid() const;
		Value GetKey() const;
		PageID GetRecordPageID() const;
		std::uint32_t GetRecordNumber() const;
		PageID GetNextLeafPageID() const;

	private:
		template<typename T>
		T GetDataByOffset(size_t offset) const;

	private:
		PageManager & m_pageManager;
		PageID m_pageID;
		size_t m_currentEntryIndex;
		std::uint32_t m_entryCount;
		size_t m_keySize;
		size_t const m_entrySize;
	};

public:
	LeafNode(PageManager & manager, PageID pageID, size_t keySize);

	EntryIterator GetEntryIterator() const;
};

template<typename T>
T LeafNode::EntryIterator::GetDataByOffset(size_t offset) const {
	char * bytes = m_pageManager.GetPage(m_pageID).lock()->GetData();
	bytes += offset;
	T data;
	BytesToNumber(bytes, data);
	return data;
}

#endif // LeafNode_h
