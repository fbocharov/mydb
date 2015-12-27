#ifndef IndexCursor_h
#define IndexCursor_h
#include "Cursor.h"

class IndexCursor : public ICursor
{
	virtual Value Get(std::string const & column) const override;
	virtual Values GetAll() const override;
	/// Returns true if there is next record and moves to it, otherwise returns false.
	virtual bool Next() override;
	virtual bool Update(std::map<std::string, Value> const & colVals) override;
	virtual bool Delete() override;
};

#endif