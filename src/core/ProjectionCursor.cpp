#include "ProjectionCursor.h"

ProjectionCursor::ProjectionCursor(std::unique_ptr<ICursor> cursor, std::vector<std::string> const& columns)
	: m_cursor(move(cursor))
	, m_fields(columns)
{}

bool ProjectionCursor::Next()
{
	return m_cursor->Next();
}

Value ProjectionCursor::Get(std::string const & column) const
{
	if (find(m_fields.begin(), m_fields.end(), column) == m_fields.end())
		throw std::runtime_error("Column \"" + column + "\" not found in this projection.");

	return m_cursor->Get(column);
}

Values ProjectionCursor::GetAll() const
{
	Values result;
	for (auto const & field : m_fields) {
		result.push_back(m_cursor->Get(field));
	}

	return result;
}

void ProjectionCursor::MoveToBegin()
{
	m_cursor->MoveToBegin();
}
