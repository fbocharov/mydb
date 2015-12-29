#ifndef InnerNode_h
#define InnerNode_h

#include <memory>

#include <backend/PageManager.h>
#include <common/Value.h>

#include "Node.h"

class InnerNode : public Node {
public:
	class KeyIterator {
	public:
		KeyIterator(PageManager & manager, PageID pageID, std::uint32_t keySize, size_t keyCount);

		KeyIterator & operator++ ();
		Value operator* () const;
		bool IsValid() const;
		PageID GetPrevPageID() const;
		PageID GetNextPageID() const;

	private:
		PageID GetIDByOffset(size_t offset) const;

	private:
		PageManager & m_pageManager;
		PageID m_pageID;
		std::uint32_t m_keySize;
		size_t m_currentKeyIndex;
		size_t m_keyCount;
	};

public:
	InnerNode(PageManager & manager, PageID pageID, size_t keySize);

	KeyIterator GetKeyIterator() const;
};

#endif // InnerNode_h
