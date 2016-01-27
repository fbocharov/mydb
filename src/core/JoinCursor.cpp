#include "JoinCursor.h"

JoinCursor::JoinCursor(std::unique_ptr<BaseCursor> left, std::unique_ptr<BaseCursor> right)
	: m_left(move(left))
	, m_right(move(right))
{}

bool JoinCursor::Next() {
	return false;
}

Value JoinCursor::Get(std::string const& column) const {
	return {};
}

Values JoinCursor::GetAll() const {
	return Values();
}

void JoinCursor::MoveToBegin() {
	m_left->MoveToBegin();
	m_right->MoveToBegin();
}
