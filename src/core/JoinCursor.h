#ifndef JoinCursor_h
#define JoinCursor_h
#include <memory>
#include "ICursor.h"

class JoinCursor : public ICursor {
public:
	JoinCursor(std::unique_ptr<ICursor> left, std::unique_ptr<ICursor> right);

	virtual bool Next() override;
	virtual Value Get(std::string const & column) const override;
	virtual Values GetAll() const override;
	virtual void MoveToBegin() override;

private:
	std::unique_ptr<ICursor> m_left;
	std::unique_ptr<ICursor> m_right;
};
#endif // JoinCursor_h