#ifndef Cursor_h
#define Cursor_h

#include <map>
#include <string>

#include <common/Value.h>

struct ICursor {
	virtual ~ICursor() = default;

	virtual Value Get(std::string const & column) const = 0;
	virtual Values GetAll() const = 0;
	/// Returns true if there is next record and moves to it, otherwise returns false.
	virtual bool Next() = 0;
	virtual bool Update(std::map<std::string, Value> const & colVals) = 0;
	virtual bool Delete() = 0;
};

#endif // Cursor_h
