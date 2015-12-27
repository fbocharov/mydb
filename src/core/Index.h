#ifndef Index_h
#define Index_h
#include <string>
#include <vector>

#include <memory>
#include "Cursor.h"
#include <common/Condition.h>

class Page;
class PageManager;

class Index
{
public:
	Index(std::string const & name, std::string const & columnName);

	static Index Deserialize(char const * data);
	void Serialize(char * data);

	// ToDo: resolve parameters
	bool Insert();

	std::unique_ptr<ICursor> GetCursor(Conditions const & conditions = Conditions());

	// ToDo: evaluate index size as summ of sizes max_tablename_length, max_columnname_length, size of padeID
	static constexpr uint32_t INDEX_SIZE = 0;

private:
	std::string m_name;
	std::string m_columnName;
};

typedef std::vector<Index> Indices;
#endif // Index_h
