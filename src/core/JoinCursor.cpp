#include "JoinCursor.h"

JoinCursor::JoinCursor(std::unique_ptr<ICursor> left, std::unique_ptr<ICursor> right, 
		std::string const& left_field, std::string const& right_field,
		std::vector<std::pair<int, std::string>> const & fields)
	: m_left(move(left))
	, m_right(move(right))
	, m_left_field(left_field)
	, m_right_field(right_field)
	, m_fields(fields)
{
	m_right->Next();
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
	Values result;
	if(m_fields.empty()) {
		for (auto const & v : m_left->GetAll())
			result.push_back(v);
		for (auto const & v : m_right->GetAll())
			result.push_back(v);
	}
	else {
		for (auto const & field : m_fields) {
			if (field.first == 1)
				result.push_back(m_left->Get(field.second));
			else
				result.push_back(m_right->Get(field.second));
		}
	}

	return result;
}

void JoinCursor::MoveToBegin() {
	m_left->MoveToBegin();
	m_right->MoveToBegin();
}

std::string const& JoinCursor::GetTableName() const {
	return m_left->GetTableName();
}

void JoinCursor::SetTableName(std::string const& value) {
	m_left->SetTableName(value);
}
