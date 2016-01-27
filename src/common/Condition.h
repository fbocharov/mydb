#ifndef Condition_h
#define Condition_h

#include <functional>
#include <vector>
#include <string>

#include "Common.h"
#include "Value.h"

using Comparator = std::function<bool(Value const &)>;

class Condition {
public:
	Condition() = default;
	template<typename T>
	Condition(std::string const & column, char op, T value);

	bool Satisfies(Value const & value) const;
	std::string const & GetColumn() const;
	char GetOperation() const;
	Value const & GetValue() const;

protected:
	std::string m_column;
	Comparator  m_comparator;
	char m_operation;
	Value m_value;
};

using Conditions = std::vector<Condition>;


struct ComparatorFactory {
	template<typename T>
	static Comparator CreateComparator(char op, T right) {
		switch (op) {
			case '=':
				return Comparator([right] (Value const & left) {
					return left.Get<decltype(right)>() == right;
				});
			case '<':
				return Comparator([right] (Value const & left) {
					return left.Get<decltype(right)>() < right;
				});
			case '>':
				return Comparator([right] (Value const & left) {
					return left.Get<decltype(right)>() > right;
				});
			default:
				throw std::runtime_error("Unknown comparator operation.");
		}
	}
};

template<typename T>
Condition::Condition(std::string const & column, char op, T value)
	: m_column(column)
	, m_comparator(ComparatorFactory::CreateComparator(op, value))
	, m_operation(op)
	, m_value(ValueType::UNKNOWN, value)
{}

#endif // Condition_h
