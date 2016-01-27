#ifndef CursorImpl_h
#define CursorImpl_h
#include <common/Value.h>
#include "DeleteNewCursor.h"
#include <common/Common.h>

class CursorImpl : public DeleteNewCursor {
public:
	virtual ~CursorImpl()
	{}

	virtual bool Next() override;
	virtual Value Get(std::string const & column) const override;
	virtual Values GetAll() const override;

	virtual bool Delete() override;
	virtual void MoveToBegin() override;
protected:
	virtual bool HasNext() const override;
	CursorImpl(ColumnDescriptors const& m_descriptors);

private:
	virtual char const * GetCurrentRecord() const = 0;
	/// Moves cursor to next record. If it was last record does nothing.
	virtual void GoToNextRecord() = 0;

private:
	ColumnDescriptors const & m_descriptors;
};
#endif // CursorImpl_h
