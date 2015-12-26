#ifndef Condition_h
#define Condition_h

#include <functional>
#include <vector>
#include <string>

class Record;
struct Value;

using Comparator = std::function<bool(Value const &)>;

class Condition {
public:
	Condition() = default;
	Condition(std::string const & column, char op, Value const & value);
	virtual ~Condition() = default;

	bool Satisfies(Record const & record) const;

protected:
	std::string m_column;
	Comparator  m_comparator;
};

using Conditions = std::vector<Condition>;


#endif // Condition_h
