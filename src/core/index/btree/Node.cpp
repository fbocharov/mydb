#include <utils/Utils.h>

#include "Node.hpp"

char * Node::InitNode(char * bytes, NodeType type, std::uint32_t size) {
	NumberToBytes(type, bytes);
	bytes += sizeof(type);

	NumberToBytes(size, bytes);
	bytes += sizeof(size);

	return bytes;
}

std::uint32_t Node::GetEntryCount() const {
	return GetValueByOffset<std::uint32_t>(m_bytes, 1);
}

bool Node::IsLeaf() const {
	return std::uint8_t(NodeType::LEAF_NODE) == GetValueByOffset<std::uint8_t>(m_bytes, 0);
}

size_t Node::CalculateEntryOffset(size_t entryNumber) const {
	return sizeof(NodeType) + sizeof(std::uint32_t) + entryNumber * m_entrySize;
}
