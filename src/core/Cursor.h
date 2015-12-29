#ifndef Cursor_h
#define Cursor_h

#include <map>
#include <string>

#include <common/Common.h>
#include <common/Condition.h>
#include <common/Value.h>

class Cursor {
public:
	Cursor(ColumnDescriptors const & descriptors);
	virtual ~Cursor() = default;

	Value Get(std::string const & column) const;
	Values GetAll() const;
	/// Returns true if there is next record and moves to it, otherwise returns false.
	virtual bool HasNext() const = 0;
	bool Next();

	virtual bool Update(std::map<std::string, Value> const & colVals) = 0;
	virtual bool Delete() = 0;

private:
	virtual char const * GetCurrentRecord() const = 0;
	/// Moves cursor to next record. If it was last record does nothing.
	virtual void GoToNextRecord() = 0;

	bool SatisfiesAll() const;

protected:
	ColumnDescriptors const & m_descriptors;
	Conditions m_conditions;
};

#endif // Cursor_h
