#include "IndexCursor.h"

Value IndexCursor::Get(std::string const& column) const
{
	return Value(ValueType::INT, 10);
}

Values IndexCursor::GetAll() const
{
	return Values();
}

bool IndexCursor::Next()
{
	return false;
}

bool IndexCursor::Update(std::map<std::string, Value> const& colVals)
{
	return true;
}

bool IndexCursor::Delete()
{
	return true;
}
