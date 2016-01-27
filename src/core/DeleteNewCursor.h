#ifndef DeleteNewCursor_h
#define DeleteNewCursor_h
#include "BaseCursor.h"

class DeleteNewCursor : public BaseCursor {
public:
	virtual bool Delete() = 0;
};
#endif // DeleteNewCursor_h
