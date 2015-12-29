#ifndef Node_h
#define Node_h

#include <memory>

#include <backend/PageManager.h>

enum class NodeType : std::uint8_t {
	INNER_NODE = 1,
	LEAF_NODE
};

class Node {
public:
	Node(PageManager & manager, PageID pageID, NodeType type, size_t keySize);
	virtual ~Node();

	bool IsLeaf() const;

private:
	void ReadHeader(char const * bytes);
	void WriteHeader(char * bytes);

protected:
	PageManager & m_pageManager;
	PageID m_pageID;
	NodeType m_type;
	size_t m_keySize;
	std::uint32_t m_keyCount;
};

#endif // Node_h
