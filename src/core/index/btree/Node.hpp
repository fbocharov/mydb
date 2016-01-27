#ifndef Node_hpp
#define Node_hpp

#include <cstdint>

#include <utils/Utils.h>

enum class NodeType : std::uint8_t {
	INNER_NODE = 1,
	LEAF_NODE
};

class Node {
public:
	Node(char * bytes, size_t entrySize)
		: m_bytes(bytes)
		, m_entrySize(entrySize)
	{}


	std::uint32_t GetEntryCount() const;
	bool IsLeaf() const;

protected:
	static void InitNode(char * bytes, NodeType type, uint32_t size);
	size_t CalculateEntryOffset(size_t entryNumber);

protected:
	char * m_bytes;
	size_t m_entrySize;
};


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

size_t Node::CalculateEntryOffset(size_t entryNumber) {
	return sizeof(NodeType) + sizeof(std::uint32_t) + entryNumber * m_entrySize;
}

#endif // Node_hpp
