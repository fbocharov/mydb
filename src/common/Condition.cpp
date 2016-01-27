#include "Condition.h"

bool Condition::Satisfies(Value const & value) const {
	return m_comparator(value);
}

std::string const & Condition::GetColumn() const {
	return m_column;
}

char Condition::GetOperation() const {
	return m_operation;
}

Value const & Condition::GetValue() const {
	return m_value;
}
