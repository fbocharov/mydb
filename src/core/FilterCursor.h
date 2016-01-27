#ifndef FilterCursor_h
#define FilterCursor_h
#include "DeleteNewCursor.h"
#include <memory>
#include <common/Condition.h>

class FilterCursor : public DeleteNewCursor {
public:
	
	FilterCursor(std::unique_ptr<DeleteNewCursor> cursor, Conditions conditions = Conditions());

	bool Next() override;
	Value Get(std::string const& column) const override;
	Values GetAll() const override;

	void MoveToBegin() override;
	
	bool Delete() override;
private:
	Conditions m_conditions;
	std::unique_ptr<DeleteNewCursor> m_cursor;
};
#endif // FilterCursor_h
