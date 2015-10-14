#ifndef Log_h
#define Log_h

#include <ostream>
#include <sstream>

enum class LogType {
	Debug,
	Info,
	Warning,
	Error
};

class Log {
	typedef std::ostream& (*EndlType)(std::ostream&);

public:
	Log(LogType type);

	static void SetStream(LogType type, std::ostream & stream);

	template<typename T>
	Log & operator<<(T const & message);
	Log & operator<<(EndlType);

private:
	std::ostream * m_stream;
	std::stringstream m_messageBuilder;

	static std::ostream * m_debugAndInfoStream;
	static std::ostream * m_warnAndErrorStream;
};


template<typename T>
Log & Log::operator<< (T const & message) {
	m_messageBuilder << message;
	return *this;
}

#endif // Log_h
