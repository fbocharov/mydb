#ifndef Page_h
#define Page_h

#include <cstddef>
#include <cstdint>

typedef std::uint32_t PageID;
static PageID constexpr INVALID_PAGE_ID = -1;

class Page {
public:
	static size_t constexpr PAGE_SIZE = 4096;

public:
	Page(PageID id, char const * data = nullptr);

	char const * GetData() const;
	char * GetData();

	void SetDirty();
	void Pin();
	void Unpin();

	bool IsPinned() const;
	bool IsDirty() const;
	PageID GetID() const;
	PageID GetPrevPageID() const;
	PageID GetNextPageID() const;
	size_t GetSpaceOffset() const;

private:
	char m_data[PAGE_SIZE];
	PageID m_id;
	bool m_isDirty;
	bool m_isPinned;
};

#endif // Page_h
