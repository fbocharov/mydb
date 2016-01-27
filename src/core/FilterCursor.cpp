#include "FilterCursor.h"

FilterCursor::FilterCursor(std::unique_ptr<DeleteCursor> cursor, Conditions conditions) 
	: m_conditions(conditions)
	, m_cursor(move(cursor))
{}

bool FilterCursor::Next() {
	while (m_cursor->Next()) {
		auto all = true;
		for (auto const & condition : m_conditions) {
			auto const & record = m_cursor->Get(condition.GetColumn());
			all &= condition.Satisfies(record);
		}

		if (all)
			return true;
	}

	return false;
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
