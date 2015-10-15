#ifndef Page_h
#define Page_h

#include <cstddef>

class Page {
public:
	void SetDirty();
	void Pin();
	void Unpin();

	bool IsPinned() const;
	bool IsDirty() const;
	size_t GetID() const;

private:
	bool m_isDirty;
	bool m_isPinned;
	size_t m_id;
};

#endif // Page_h
