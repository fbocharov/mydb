#include "FilterCursor.h"

FilterCursor::FilterCursor(std::unique_ptr<InternalCursor> cursor, Conditions conditions) 
	: m_conditions(conditions)
	, m_cursor(move(cursor))
{}

bool FilterCursor::Next() {
	while (m_cursor->Next()) {
		auto all = true;
		for (int i = 0; all && i < m_conditions.size(); ++i) {
			auto const & record = m_cursor->Get(m_conditions[i].GetColumn());
			all &= m_conditions[i].Satisfies(record);
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

PageID FilterCursor::GetCurrentPage() const {
	return m_cursor->GetCurrentPage();
}

std::uint32_t FilterCursor::GetCurrentRecordNumber() const {
	return m_cursor->GetCurrentRecordNumber();
}

bool FilterCursor::Delete() {
	return m_cursor->Delete();
}
