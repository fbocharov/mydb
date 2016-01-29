#ifndef LeafNode_hpp
#define LeafNode_hpp

#include <cstddef>
#include <stdexcept>

#include <backend/Page.h>
#include <utils/Utils.h>

#include "Node.hpp"

/**
 * Leaf node sturcture looks as follows:
 *
 * node_type|key_count|key_1|flags_1|record_page_id_1|record_number_1|...|key_N|flags_N|record_page_id_N|record_number_N
 *
 * node_type -- should be 2 (see NodeType::LEAF_NODE) for leaf nodes
 * key_i -- value of key
 * flags_i -- flags for this key (deleted or not)
 * record_page_id_i -- page id where record is placed
 * record_number_i -- record number on that page
 */

/// This is just wrapper over raw bytes. It does not perform any checks for
/// overflowing page size and so on -- all of them must be prerformed in tree.
template<typename KeyT>
class LeafNode : public Node {
public:
	LeafNode(char * bytes)
		: Node(bytes, sizeof(KeyT) + sizeof(std::uint8_t) + sizeof(PageID) + sizeof(std::uint32_t))
	{}

	static void InitNode(char * bytes);

	KeyT GetKey(size_t entryNumber) const;
	PageID GetRecordPage(size_t entryNumber) const;
	std::uint32_t GetRecordNumber(size_t entryNumber) const;
	bool RecordIsDeleted(size_t entryNumber) const;

	void Insert(KeyT key, PageID pageID, std::uint32_t recordNumber, bool unique);
	KeyT TransferHalf(char * nodeBytes);
	void MarkDeleted(size_t entryNumber);
};


template<typename KeyT>
void LeafNode<KeyT>::InitNode(char * bytes) {
	Node::InitNode(bytes, NodeType::LEAF_NODE, 0);
}

template<typename KeyT>
KeyT LeafNode<KeyT>::GetKey(size_t entryNumber) const {
	size_t offset = CalculateEntryOffset(entryNumber);
	return GetValueByOffset<KeyT>(m_bytes, offset);
}

template<typename KeyT>
PageID LeafNode<KeyT>::GetRecordPage(size_t entryNumber) const {
	size_t offset = CalculateEntryOffset(entryNumber) +
		sizeof(KeyT) + sizeof(std::uint8_t);
	return GetValueByOffset<PageID>(m_bytes, offset);
}

template<typename KeyT>
std::uint32_t LeafNode<KeyT>::GetRecordNumber(size_t entryNumber) const {
	size_t offset = CalculateEntryOffset(entryNumber) +
		sizeof(KeyT) + sizeof(std::uint8_t) + sizeof(PageID);
	return GetValueByOffset<std::uint32_t>(m_bytes, offset);
}

template<typename KeyT>
bool LeafNode<KeyT>::RecordIsDeleted(size_t entryNumber) const {
	size_t offset = CalculateEntryOffset(entryNumber) + sizeof(KeyT);
	return GetValueByOffset<std::uint8_t>(m_bytes, offset);
}

template<typename KeyT>
void LeafNode<KeyT>::MarkDeleted(size_t entryNumber) {
	size_t offset = CalculateEntryOffset(entryNumber) + sizeof(KeyT);
	NumberToBytes<std::uint8_t>(1, m_bytes + offset);
}

template<typename KeyT>
void LeafNode<KeyT>::Insert(KeyT key, PageID pageID, uint32_t recordNumber, bool unique) {
	size_t keyCount = GetEntryCount();
	size_t i = 0;
	char * bytes = m_bytes + CalculateEntryOffset(0);
	for (; i < keyCount; ++i) {
		KeyT currentKey = GetValueByOffset<KeyT>(bytes, 0);
		if (currentKey == key) {
			bool deleted = GetValueByOffset<std::uint8_t>(bytes, sizeof(KeyT));

			if (unique && !deleted)
				throw std::runtime_error("Record with value " + std::to_string(key) + " already exist.");

			if (deleted) { // if there is entry with eq deleted key -> can insert right here
				bytes += sizeof(KeyT);

				NumberToBytes<std::uint8_t>(0, bytes);
				bytes += sizeof(std::uint8_t);

				NumberToBytes(pageID, bytes);
				bytes += sizeof(PageID);

				NumberToBytes(recordNumber, bytes);
				return;
			}
		}

		if (currentKey > key)
			break;
		bytes += m_entrySize;
	}

	memmove(bytes + m_entrySize, bytes, (keyCount - i) * m_entrySize);

	NumberToBytes<KeyT>(key, bytes);
	bytes += sizeof(key);

	NumberToBytes<std::uint8_t>(0, bytes);
	bytes += sizeof(std::uint8_t);

	NumberToBytes<PageID>(pageID, bytes);
	bytes += sizeof(PageID);

	NumberToBytes<std::uint32_t>(recordNumber, bytes);

	Node::InitNode(m_bytes, NodeType::LEAF_NODE, ++keyCount);
}

template<typename KeyT>
KeyT LeafNode<KeyT>::TransferHalf(char * nodeBytes) {
	size_t keyCount = GetEntryCount();

	size_t transEntryCount = keyCount / 2;
	nodeBytes = Node::InitNode(nodeBytes, NodeType::LEAF_NODE, transEntryCount);

	// moving entries
	size_t offset = CalculateEntryOffset(keyCount - transEntryCount);
	memmove(nodeBytes, m_bytes + offset, transEntryCount * m_entrySize);

	// updating current node size
	Node::InitNode(m_bytes, NodeType::LEAF_NODE, keyCount - transEntryCount);

	// returning last key left in current node
	offset = CalculateEntryOffset((keyCount - 1) / 2);
	return GetValueByOffset<KeyT>(m_bytes, offset);
}

#endif // LeafNode_hpp
