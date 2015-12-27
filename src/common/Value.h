#ifndef Value_h
#define Value_h

#include <string>
#include <vector>

#include <utils/Utils.h>

enum class ValueType : std::uint8_t {
	UNKNOWN,
	INT,
	DOUBLE,
	VARCHAR
};

class Value {
public:
	Value() = default;
	template<typename T>
	Value(ValueType type, T value);
	Value(ValueType type, std::string const & str);

	template<typename T>
	T Get() const;
	ValueType GetType() const;
	size_t GetSize() const;
	char const * GetBytes() const;
	bool IsEmpty() const;
	std::string ToString() const;

private:
	ValueType m_type;
	// NOTE: string here used as buffer either for numbers bytes
	// or for varchar bytes.
	std::string m_bytes;
	size_t m_size;
};

typedef std::vector<Value> Values;

template<>
inline std::string Value::Get<std::string>() const {
	return m_bytes;
}

template<typename T>
Value::Value(ValueType type, T value)
	: m_type(type)
	, m_size(sizeof(T))
{
	char buf[sizeof(T)];
	NumberToBytes(value, buf);
	m_bytes = std::string(buf, sizeof(T));
}

template<typename T>
T Value::Get() const {
	T value;
	BytesToNumber(m_bytes.data(), value);
	return value;
}

#endif // Value_h
