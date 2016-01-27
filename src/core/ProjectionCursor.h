#include "ICursor.h"
#include <memory>

class ProjectionCursor : ICursor {
public:
	ProjectionCursor(std::unique_ptr<ICursor> cursor, std::vector<std::string> const & columns);
	
	virtual bool Next() override;
	virtual Value Get(std::string const& column) const override;
	virtual Values GetAll() const override;
	virtual void MoveToBegin() override;
	
private:
	std::unique_ptr<ICursor> m_cursor;
	std::vector<std::string> m_fields;
};
