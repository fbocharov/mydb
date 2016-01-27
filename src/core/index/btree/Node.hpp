#ifndef Node_hpp
#define Node_hpp

#include <cstdint>
#include <cstddef>

enum class NodeType : std::uint8_t {
	INNER_NODE = 1,
	LEAF_NODE
};

class Node {
public:
	Node(char * bytes, size_t entrySize = 0)
		: m_bytes(bytes)
		, m_entrySize(entrySize)
	{}

	std::uint32_t GetEntryCount() const;
	bool IsLeaf() const;

protected:
	static char * InitNode(char * bytes, NodeType type, uint32_t size);
	size_t CalculateEntryOffset(size_t entryNumber) const;

protected:
	char * m_bytes;
	size_t m_entrySize;
};

#endif // Node_hpp
