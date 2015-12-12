#ifndef Condition_h
#define Condition_h

#include <functional>
#include <string>

class Record;
struct Value;

typedef std::function<bool(Value const &)> Comparator;

class Condition {
public:
	Condition() = default;
	Condition(std::string const & column, char op, Value const & value);
	virtual ~Condition() = default;

	bool Satisfies(Record const & record) const;
	bool IsEmpty() const;

protected:
	std::string m_column;
	Comparator  m_comparator;
};


#endif // Condition_h
