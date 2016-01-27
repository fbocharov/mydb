#ifndef DeleteCursor_h
#define DeleteCursor_h
#include "Cursor.h"

class DeleteCursor : public Cursor {
public:
	DeleteCursor(ColumnDescriptors const & descriptors);
	virtual bool Delete() = 0;
};
#endif // DeleteCursor_h
