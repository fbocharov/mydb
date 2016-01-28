#ifndef BaseCursor_h
#define BaseCursor_h
#include <common/Value.h>

class ICursor {
public:
	virtual ~ICursor() = default;

	virtual bool Next() = 0;
	virtual Value Get(std::string const & column) const = 0;
	virtual Values GetAll() const = 0;
	virtual std::string const & GetTableName() const = 0;
	virtual void SetTableName(std::string const & value) = 0;

	virtual void MoveToBegin() = 0;
};
#endif // BaseCursor_h
