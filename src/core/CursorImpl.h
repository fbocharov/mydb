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

protected:
	CursorImpl(ColumnDescriptors const& m_descriptors);

private:
	virtual bool HasNext() const = 0;
	virtual char const * GetCurrentRecord() const = 0;
	/// Moves cursor to next record. If it was last record does nothing.
	virtual void GoToNextRecord() = 0;

private:
	ColumnDescriptors const & m_descriptors;
};
#endif // CursorImpl_h
