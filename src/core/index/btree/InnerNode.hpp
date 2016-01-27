#ifndef InnerNode_hpp
#define InnerNode_hpp

#include <cstddef>

#include <backend/Page.h>
#include <utils/Utils.h>

#include "Node.hpp"

/**
 * Inner node sturcture looks as follows:
 *
 * node_type|key_count|page_id|key_1|page_id|key_2|...|page_id|key_N|page_id
 *
 * node_type -- should be 1 (see NodeType::INNER_NODE) for inner nodes
 * key_i -- value of key
 * page_id -- pageID of child node
 */


/// This is just wrapper over raw bytes. It does not perform any checks for
/// overflowing page size and so on -- all of them must be prerformed in tree.
template<typename KeyT>
class InnerNode : public Node {
public:
	InnerNode(char * bytes)
		: Node(bytes, sizeof(PageID) + sizeof(KeyT))
	{}

	static void InitNode(char * bytes, KeyT key, PageID left, PageID right);

	PageID GetNextNodePage(KeyT key) const;

	void Insert(KeyT key, PageID right);
	KeyT TransferHalf(char * nodeBytes);

private:
	size_t FindFirstGEEntry(KeyT key) const;
};

template<typename KeyT>
void InnerNode<KeyT>::InitNode(char * bytes, KeyT key, PageID left, PageID right) {
	bytes = Node::InitNode(bytes, NodeType::INNER_NODE, 1);

	NumberToBytes(left, bytes);
	bytes += sizeof(left);

	NumberToBytes(key, bytes);
	bytes += sizeof(key);

	NumberToBytes(right, bytes);
}

template<typename KeyT>
PageID InnerNode<KeyT>::GetNextNodePage(KeyT key) const {
	size_t geKeyNumber = FindFirstGEEntry(key);
	size_t offset = CalculateEntryOffset(geKeyNumber);

	return GetValueByOffset<PageID>(m_bytes, offset);
}

template<typename KeyT>
void InnerNode<KeyT>::Insert(KeyT key, PageID right) {
	size_t entryCount = GetEntryCount();

	size_t geEntry = FindFirstGEEntry(key);
	size_t offset = CalculateEntryOffset(geEntry) + sizeof(PageID);

	memmove(m_bytes + offset + m_entrySize, m_bytes + offset, m_entrySize);

	NumberToBytes(key, m_bytes + offset);
	offset += sizeof(KeyT);

	NumberToBytes(right, m_bytes + offset);

	Node::InitNode(m_bytes, NodeType::INNER_NODE, ++entryCount);
}

template<typename KeyT>
KeyT InnerNode<KeyT>::TransferHalf(char * nodeBytes) {
	size_t keyCount = GetEntryCount();

	size_t transKeysCount = (keyCount - 1) / 2;
	nodeBytes = Node::InitNode(nodeBytes, NodeType::INNER_NODE, transKeysCount);

	// moving entries
	size_t offset = CalculateEntryOffset(keyCount - transKeysCount);
	memmove(nodeBytes, m_bytes + offset, transKeysCount * m_entrySize + sizeof(PageID));

	// updating current node size
	Node::InitNode(m_bytes, NodeType::INNER_NODE, keyCount - transKeysCount - 1);

	// returning key that was in between left and moved parts
	offset = CalculateKeyOffset(keyCount / 2);
	return GetValueByOffset<KeyT>(m_bytes, offset);
}

template<typename KeyT>
size_t InnerNode<KeyT>::FindFirstGEEntry(KeyT key) const {
	size_t entryCount = GetEntryCount();
	char * bytes = m_bytes + CalculateEntryOffset(0);

	for (size_t i = 0; i < entryCount; ++i) {
		KeyT currentKey = GetValueByOffset<KeyT>(bytes, sizeof(PageID));
		if (currentKey >= key)
			return i;
		bytes += m_entrySize;
	}

	return entryCount;
}

#endif // InnerNode_hpp
