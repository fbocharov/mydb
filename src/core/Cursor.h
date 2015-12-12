#ifndef Cursor_h
#define Cursor_h

class Record;

struct ICursor {
	virtual ~ICursor() = default;
	virtual Record const & Get() const = 0;
	/// Returns true if there is next record and moves to it, otherwise returns false.
	virtual bool Next() = 0;
	virtual bool Update(std::map<std::string, Value> const & colVals) = 0;
	virtual bool Delete() = 0;
};

#endif // Cursor_h
