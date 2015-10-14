#include <ctime>

#include "Log.h"

std::ostream * Log::m_debugAndInfoStream = nullptr;
std::ostream * Log::m_warnAndErrorStream = nullptr;

Log::Log(LogType type) {
	switch (type) {
		case LogType::Debug:
			#ifndef DEBUG
				m_stream = nullptr;
				break;
			#endif
		case LogType::Info:
			m_stream = m_debugAndInfoStream;
			break;
		case LogType::Warning:
		case LogType::Error:
			m_stream = m_warnAndErrorStream;
			break;
	}
}

Log & Log::operator<< (EndlType) {
	char timeAndDate[20];
	time_t now = time(NULL);
	strftime(timeAndDate, 20, "%F %T", localtime(&now));
	// If logging is enabled and set -> writing.
	if (m_stream)
		*m_stream << timeAndDate << ": " << m_messageBuilder.str() << std::endl;
	else
	// Else just clearing message buffer.
		m_messageBuilder.str("");
	return *this;
}

void Log::SetStream(LogType type, std::ostream & stream) {
	switch (type) {
		case LogType::Debug:
		case LogType::Info:
			m_debugAndInfoStream = &stream;
			break;
		case LogType::Warning:
		case LogType::Error:
			m_warnAndErrorStream = &stream;
			break;
	}
}
