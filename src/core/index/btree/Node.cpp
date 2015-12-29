#include <utils/Utils.h>

#include "Node.h"

Node::Node(PageManager & manager, PageID pageID, NodeType type, size_t keySize)
	: m_pageManager(manager)
	, m_pageID(pageID)
	, m_type(type)
	, m_keySize(keySize)
{
	auto page = m_pageManager.GetPage(m_pageID).lock();
	char * bytes = page->GetData();
	if (!*bytes)
		NumberToBytes(m_type, bytes);
	else
		ReadHeader(bytes);
}

Node::~Node() {
	auto page = m_pageManager.GetPage(m_pageID).lock();
	WriteHeader(page->GetData());
	page->Unpin();
}

bool Node::IsLeaf() const {
	return m_type == NodeType::LEAF_NODE;
}

void Node::ReadHeader(char const * bytes) {
	NodeType type;
	BytesToNumber(bytes, type);
	if (type != m_type)
		throw std::runtime_error("Creating inner index node from bad page.");
	bytes += sizeof(type);

	BytesToNumber(bytes, m_keyCount);
	bytes += sizeof(m_keyCount);
}

void Node::WriteHeader(char * bytes) {
	NumberToBytes(m_type, bytes);
	bytes += sizeof(NodeType);

	NumberToBytes(m_keyCount, bytes);
	bytes += sizeof(m_keyCount);
}
