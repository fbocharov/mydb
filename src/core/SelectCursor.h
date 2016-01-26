#ifndef SelectCursor_h
#define SelectCursor_h
#include <common/Common.h>
#include <common/Condition.h>
#include <backend/Page.h>

class PageManager;

class SelectCursor {
public:
	virtual ~SelectCursor() = default;

	SelectCursor(ColumnDescriptors const & descriptors);

	SelectCursor(PageManager & pageManager, PageID startPageID, ColumnDescriptors const & descriptors,
		Conditions const & conditions, std::vector<std::string> const & fields = std::vector<std::string>());

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
#endif // SelectCursor_h 
