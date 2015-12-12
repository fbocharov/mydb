#include "Common.h"
#include "Condition.h"
#include "Record.h"

#include <iostream>

namespace {
	Comparator CreateComparator(char op, Value const & right) {
		switch (op) {
			case '=':
				return Comparator([right] (Value const & left) {
//					bool v1 = right.type == left.type;
//					bool v2 = right.value == left.value;
//					std::cerr << v1 << ", " << v2 << std::endl;
//					std::cerr << right.value.length() << ", " << left.value.length() << std::endl;
//					std::cerr << (int)right.type << "==" << (int)left.type << std::endl;
//					std::cerr << right.value << "==" << left.value << std::endl;
//					return v1 && v2;
					return left.type == right.type && left.value == right.value;
				});
			case '<':
				return Comparator([right] (Value const & left) {
					return left.type == right.type && left.value < right.value;
				});
			case '>':
				return Comparator([right] (Value const & left) {
					return left.type == right.type && left.value > right.value;
				});
			default:
				throw std::runtime_error("Unknown comparator operation.");
		}
	}
}

Condition::Condition(std::string const & fieldName, char op, Value const & value)
	: m_column(fieldName)
	, m_comparator(CreateComparator(op, value))
{}

bool Condition::IsEmpty() const {
	return m_column.empty();
}

bool Condition::Satisfies(Record const & record) const {
	return m_comparator(record[m_column]);
}
