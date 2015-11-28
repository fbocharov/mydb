#include <backend/PageManager.h>
#include <utils/Log.h>
#include <utils/Utils.h>

#include "Table.h"
#include "FullScanCursor.h"

using std::uint32_t;

Table::Table(PageManager & manager, ColumnDescriptors const & descriptors)
	: m_columnDescriptors(descriptors)
	, m_pageManager(manager)
{
	auto firstPage = m_pageManager.AllocatePage().lock();
	m_firstPageID = firstPage->GetID();
	firstPage->SetNextPageID(INVALID_PAGE_ID);
	firstPage->SetPrevPageID(firstPage->GetID());
	m_pageWithSpace = std::make_unique<DataPage>(m_pageManager, m_firstPageID, m_columnDescriptors);
}

Table::Table(ColumnDescriptors const & columnDescriptors, PageManager & pageManager, PageID firstPage)
	: m_columnDescriptors(columnDescriptors)
	, m_pageManager(pageManager)
	, m_firstPageID(firstPage)
{
	PageID lastPageID = m_pageManager.GetPage(firstPage).lock()->GetPrevPageID();
	m_pageWithSpace = std::make_unique<DataPage>(m_pageManager, lastPageID, m_columnDescriptors);
}

std::unique_ptr<Table> Table::Deserialize(Page const & page, PageManager & manager) {
	char const * data = page.GetData();

	uint32_t fieldsCount = 0;
	BytesToNumber(data, fieldsCount);
	data += 4;
	std::vector<ColumnDescriptor> descriptors;
	for (uint32_t i = 0; i < fieldsCount; ++i) {
		ColumnDescriptor descriptor = ColumnDescriptor::Deserialize(data);
		descriptors.emplace_back(descriptor);
		data += ColumnDescriptor::DESCRIPTOR_SIZE;
	}

	PageID firstPageID = 0;
	BytesToNumber(data, firstPageID);

	return std::make_unique<Table>(descriptors, manager, firstPageID);
}

void Table::Serialize(Page & page) {
	char * data = page.GetData();

	uint32_t descriptorCount = m_columnDescriptors.size();
	NumberToBytes(descriptorCount, data);
	data += 4;
	for (size_t i = 0; i < m_columnDescriptors.size(); ++i) {
		m_columnDescriptors[i].Serialize(data);
		data += ColumnDescriptor::DESCRIPTOR_SIZE;
	}

	NumberToBytes(m_firstPageID, data);
}

void Table::Insert(std::map<std::string, std::string> const & colVals) {
	if (colVals.size() != m_columnDescriptors.size()) {
		Log(LogType::Error) << "Trying to insert record of length " << colVals.size()
							<< " into table with " << m_columnDescriptors.size() << " columns."
							<< std::endl;
		throw std::logic_error("Incorrect number of fields.");
	}

	if (!m_pageWithSpace->HasFreeSpace())
		AddPage();

	m_pageWithSpace->AppendRecord(colVals);
}

std::unique_ptr<ICursor> Table::GetCursor() {
	return std::make_unique<FullScanCursor>(m_pageManager, m_firstPageID, m_columnDescriptors);
}

void Table::AddPage() {
	auto firstPage = m_pageManager.GetPage(m_firstPageID).lock();
	auto lastPage = m_pageManager.GetPage(firstPage->GetPrevPageID()).lock();
	auto newPage = m_pageManager.AllocatePage().lock();

	newPage->SetNextPageID(lastPage->GetNextPageID());
	newPage->SetPrevPageID(lastPage->GetID());
	PageID newPageID = newPage->GetID();
	firstPage->SetPrevPageID(newPageID);
	lastPage->SetNextPageID(newPageID);

	m_pageWithSpace = std::make_unique<DataPage>(m_pageManager, newPageID, m_columnDescriptors);
}
