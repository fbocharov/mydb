#ifndef Page_h
#define Page_h

#include <cstdint>

typedef std::uint32_t PageID;
static PageID constexpr INVALID_PAGE_ID = -1;

class HeapFile;

class Page {
public:
	static size_t constexpr PAGE_SIZE = 4096;
	static size_t constexpr PAGE_DATA_SIZE = PAGE_SIZE - 2 * sizeof(PageID);

public:
	Page(PageID id, char const * data = nullptr);

	char const * GetData() const;
	char * GetData();

	void SetDirty();
	void Pin();
	void Unpin();
	void SetPrevPageID(PageID id);
	void SetNextPageID(PageID id);

	bool IsPinned() const;
	bool IsDirty() const;
	PageID GetID() const;
	PageID GetPrevPageID() const;
	PageID GetNextPageID() const;

private:
	char m_data[PAGE_SIZE];
	PageID m_id;
	bool m_isDirty;
	bool m_isPinned;

	friend class HeapFile;
};

#endif // Page_h
