#include "Index.h"

Index::Index(std::string const & name, std::string const & columnName)
	: m_name(name)
	, m_columnName(columnName)
{
}

Index Index::Deserialize(char const * data)
{
	return Index("", "");
}

void Index::Serialize(char* data)
{

}
