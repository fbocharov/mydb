#ifndef Cursor_h
#define Cursor_h
#include <common/Common.h>
#include <common/Condition.h>
#include <backend/Page.h>

class PageManager;

class Cursor {
public:
	virtual ~Cursor() = default;

	Cursor(ColumnDescriptors const & descriptors);

	Value Get(std::string const & column) const;
	Values GetAll() const;
	/// Returns true if there is next record and moves to it, otherwise returns false.
	virtual bool HasNext() const = 0;
	bool Next();

protected:
	ColumnDescriptors const & m_descriptors;
	std::vector<std::string> const & m_fields;
	Conditions m_conditions;

private:
	virtual char const * GetCurrentRecord() const = 0;
	/// Moves cursor to next record. If it was last record does nothing.
	virtual void GoToNextRecord() = 0;

	bool SatisfiesAll() const;
};
#endif // Cursor_h 
