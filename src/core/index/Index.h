#ifndef Index_hpp
#define Index_hpp

#include <memory>

#include <common/Value.h>
#include <common/Condition.h>
#include <backend/Page.h>
#include <utils/Utils.h>

enum class IndexType : std::uint8_t {
	BTREE,
	HASH
};

class InternalCursor;
class PageManager;

class Index {
public:
	Index(bool isUnique, IndexType type);
	virtual ~Index() = default;

	virtual char * Serialize(char * bytes) const = 0;

	IndexType GetType() const;
	bool IsUnique() const;
	virtual bool Insert(Value const & key, PageID pageID, std::uint32_t recordNumber) = 0;
	virtual std::unique_ptr<InternalCursor> GetCursor(ColumnDescriptors const & descriptors,
		Condition const & from, Condition const & to) = 0;

protected:
	bool const m_isUnique;
	IndexType m_type;
};

#endif // Index_hpp
