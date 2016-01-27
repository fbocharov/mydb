#include "JoinCursor.h"

JoinCursor::JoinCursor(std::unique_ptr<ICursor> left, std::unique_ptr<ICursor> right, std::string const& left_field, std::string const& right_field)
	: m_left(move(left))
	, m_right(move(right))
	, m_left_field(left_field)
	, m_right_field(right_field)
{
	m_right->Next();
	m_left->Next();
}

bool JoinCursor::Next() {
	while (true) {
		auto lValue = m_left->Get(m_left_field);
		while (m_right->Next()) {
			if (lValue == m_right->Get(m_right_field)) {
				return true;
			}
		}
		
		if (!m_left->Next())
			break;
		m_right->MoveToBegin();
		m_right->Next();
	}
	
	return false;
}

Value JoinCursor::Get(std::string const& column) const {
	try {
		return m_left->Get(column);
	}
	catch(std::runtime_error const &)
	{}

	return m_right->Get(column);
}

Values JoinCursor::GetAll() const {
	auto result = m_left->GetAll();
	for (auto const & rvalue : m_right->GetAll())
		result.push_back(rvalue);

	return result;
}

void JoinCursor::MoveToBegin() {
	m_left->MoveToBegin();
	m_right->MoveToBegin();
}
