#ifndef FilterCursor_h
#define FilterCursor_h

#include <memory>
#include <common/Condition.h>

#include "InternalCursor.h"

class FilterCursor : public InternalCursor {
public:
	FilterCursor(std::unique_ptr<InternalCursor> cursor, Conditions conditions = Conditions());

	virtual bool Next() override;
	virtual Value Get(std::string const& column) const override;
	virtual Values GetAll() const override;

	virtual PageID GetCurrentPage() const override;
	virtual std::uint32_t GetCurrentRecordNumber() const override;
	virtual bool Delete() override;
	virtual void MoveToBegin() override;

	virtual std::string const& GetTableName() const override;
	virtual void SetTableName(std::string const& value) override;

private:
	Conditions m_conditions;
	std::unique_ptr<InternalCursor> m_cursor;
};
#endif // FilterCursor_h
