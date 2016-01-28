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

Condition Condition::CopyWithNewColName(std::string const& colname) const
{
	Condition newCondition;
	newCondition.m_column = colname;
	newCondition.m_comparator = m_comparator;
	newCondition.m_operation = m_operation;
	newCondition.m_value = m_value;

	return newCondition;
}
