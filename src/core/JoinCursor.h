#ifndef JoinCursor_h
#define JoinCursor_h
#include "BaseCursor.h"
#include <memory>

class JoinCursor : public BaseCursor {
public:
	JoinCursor(std::unique_ptr<BaseCursor> left, std::unique_ptr<BaseCursor> right);

	virtual bool Next() override;
	virtual Value Get(std::string const & column) const override;
	virtual Values GetAll() const override;
	virtual void MoveToBegin() override;

protected:
	virtual bool HasNext() const override;

private:
	std::unique_ptr<BaseCursor> m_left;
	std::unique_ptr<BaseCursor> m_right;
};
#endif // JoinCursor_h