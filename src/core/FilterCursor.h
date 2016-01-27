#ifndef FilterCursor_h
#define FilterCursor_h
#include <memory>
#include <common/Condition.h>
#include "DeleteCursor.h"

class FilterCursor : public DeleteCursor {
public:
	FilterCursor(std::unique_ptr<DeleteCursor> cursor, Conditions conditions = Conditions());

	virtual bool Next() override;
	virtual Value Get(std::string const& column) const override;
	virtual Values GetAll() const override;

	virtual void MoveToBegin() override;
	
	virtual bool Delete() override;
private:
	Conditions m_conditions;
	std::unique_ptr<DeleteCursor> m_cursor;
};
#endif // FilterCursor_h
