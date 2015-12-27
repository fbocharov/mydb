#include <stdexcept>

#include "Value.h"

Value::Value(ValueType type, std::string const & str)
	: m_type(type)
{
	switch (type) {
		case ValueType::INT:
		case ValueType::DOUBLE:
			m_bytes = str;
			m_size = str.size();
			break;
		case ValueType::VARCHAR:
			m_bytes = str.c_str();
			m_size = m_bytes.size() + 1;
			break;
		default:
			throw std::runtime_error("Unknown value type.");
	}
}

template<>
std::string Value::Get<std::string>() const {
	return m_bytes;
}

ValueType Value::GetType() const {
	return m_type;
}

size_t Value::GetSize() const {
	return m_size;
}

char const * Value::GetBytes() const {
	return m_bytes.data();
}

bool Value::IsEmpty() const {
	return m_bytes.empty();
}

std::string Value::ToString() const {
	switch (m_type) {
		case ValueType::INT: {
			int value;
			BytesToNumber(m_bytes.data(), value);
			return std::to_string(value);
		}
		case ValueType::DOUBLE: {
			double value;
			BytesToNumber(m_bytes.data(), value);
			return std::to_string(value);
		}
		case ValueType::VARCHAR: {
			return m_bytes;
		}
		default:
			return "unknown";
	}
}