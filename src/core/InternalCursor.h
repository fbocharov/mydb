#ifndef InternalCursor_h
#define InternalCursor_h

#include <backend/Page.h>

#include "ICursor.h"

class InternalCursor : public ICursor {
public:
	virtual ~InternalCursor() = default;
	virtual bool Delete() = 0;
	virtual PageID GetCurrentPage() const = 0;
	virtual std::uint32_t GetCurrentRecordNumber() const = 0;
};

#endif // InternalCursor_h
