#include "CursorImpl.h"

CursorImpl::CursorImpl(ColumnDescriptors const& descriptors)
	: m_descriptors(descriptors)
{}

bool CursorImpl::Next() {
	return false;
}

Value CursorImpl::Get(std::string const& column) const {
	return Value();
}

Values CursorImpl::GetAll() const {
	return Values();
}

bool CursorImpl::Delete() {
	return false;
}

void CursorImpl::MoveToBegin() {
}

bool CursorImpl::HasNext() const {
	return false;
}
