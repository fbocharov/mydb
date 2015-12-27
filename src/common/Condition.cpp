#include "Condition.h"

bool Condition::Satisfies(Value const & value) const {
	return m_comparator(value);
}

std::string const & Condition::GetColumn() const {
	return m_column;
}
