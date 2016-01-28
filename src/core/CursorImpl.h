#ifndef CursorImpl_h
#define CursorImpl_h

#include <common/Value.h>
#include <common/Common.h>

#include "InternalCursor.h"

class CursorImpl : public InternalCursor {
public:
	virtual bool Next() override;
	virtual Value Get(std::string const & column) const override;
	virtual Values GetAll() const override;
	virtual std::string const & GetTableName() const override;
	virtual void SetTableName(std::string const& value) override;

protected:
	CursorImpl(ColumnDescriptors const & m_descriptors);

private:
	virtual bool HasNext() const = 0;
	virtual char const * GetCurrentRecord() const = 0;
	/// Moves cursor to next record. If it was last record does nothing.
	virtual void GoToNextRecord() = 0;

protected:
	ColumnDescriptors const & m_descriptors;
	std::string m_tableName;
};
#endif // CursorImpl_h
