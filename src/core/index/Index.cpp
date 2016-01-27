#include "Index.h"

Index::Index(bool isUnique, IndexType type)
	: m_isUnique(isUnique)
	, m_type(type)
{}

IndexType Index::GetType() const {
	return m_type;
}

bool Index::IsUnique() const {
	return m_isUnique;
}
