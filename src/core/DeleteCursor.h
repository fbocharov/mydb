#ifndef DeleteCursor_h
#define DeleteCursor_h
#include "SelectCursor.h"

class DeleteCursor : public SelectCursor {
public:
	DeleteCursor(ColumnDescriptors const & descriptors);
	virtual bool Delete() = 0;
};
#endif // DeleteCursor_h
