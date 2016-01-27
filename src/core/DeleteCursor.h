#ifndef DeleteCursor_h
#define DeleteCursor_h
#include "ICursor.h"

class DeleteCursor : public ICursor {
public:
	virtual bool Delete() = 0;
};
#endif // DeleteNewCursor_h
