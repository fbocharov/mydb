#ifndef Index_hpp
#define Index_hpp

#include <memory>

#include <common/Value.h>
#include <common/Condition.h>
#include <backend/Page.h>

enum class IndexType : std::uint8_t {
	BTREE,
	HASH
};

class Cursor;

class IIndex {
public:
	IIndex(bool isUnique)
		: m_isUnique(isUnique)
	{}
	virtual ~IIndex() = default;

	virtual bool Insert(Value const & key, PageID pageID, std::uint32_t recordNumber) = 0;
	virtual std::unique_ptr<Cursor> GetCursor(Condition const & condition) = 0;

protected:
	bool const m_isUnique;
};

#endif // Index_hpp
