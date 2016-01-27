#ifndef Index_hpp
#define Index_hpp

#include <memory>

#include <common/Value.h>
#include <common/Condition.h>
#include <backend/Page.h>
#include <core/BaseCursor.h>

enum class IndexType : std::uint8_t {
	BTREE,
	HASH
};

class IIndex {
public:
	IIndex(std::string const & name, bool isUnique);
	virtual ~IIndex() = default;

	virtual bool Insert(Value key, PageID pageID, std::uint32_t recordNumber) = 0;
	virtual size_t RemoveSatisfying(Condition const & condition) = 0;
	virtual std::unique_ptr<ICursor> GetCursor(Condition const & condition) = 0;

	std::string const & GetName() const {
		return m_name;
	}

protected:
	bool const m_isUnique;

private:
	std::string const m_name;
};

template<typename T>
IIndex<T>::IIndex(std::string const & name, bool isUnique)
	: m_isUnique(isUnique)
	, m_name(name)
{}


#endif // Index_hpp
