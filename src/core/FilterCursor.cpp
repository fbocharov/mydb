#include "FilterCursor.h"

FilterCursor::FilterCursor(std::unique_ptr<DeleteNewCursor> cursor, Conditions conditions) 
	: m_conditions(conditions)
	, m_cursor(move(cursor))
{}

bool FilterCursor::Next() {
	return m_cursor->Next();
}

Value FilterCursor::Get(std::string const& column) const {
	return m_cursor->Get(column);
}

Values FilterCursor::GetAll() const {
	return m_cursor->GetAll();
}

void FilterCursor::MoveToBegin() {
	m_cursor->MoveToBegin();
}

bool FilterCursor::Delete() {
	return m_cursor->Delete();
}

bool FilterCursor::HasNext() const {
	return false;
}
