#ifndef Cursor_h
#define Cursor_h

#include "Record.h"

struct ICursor {
	virtual ~ICursor() = default;
	virtual Record Get() = 0;
	/// Returns true if there is next record and moves to it.
	virtual bool Next() = 0;
};

#endif // Cursor_h
