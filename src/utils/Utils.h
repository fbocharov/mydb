#ifndef Utils_h
#define Utils_h

#include <cstddef>
#include <cstring>

// Playing with fire here!

template<typename T>
inline void BytesToNumber(char const * bytes, T & number) {
	number = *(reinterpret_cast<T const *>(bytes));
}

template<typename T>
inline void NumberToBytes(T number, char * bytes) {
	char const * numberPtr = reinterpret_cast<char const *>(&number);
	::memcpy(bytes, numberPtr, sizeof(T));
}

template<typename T>
T GetValueByOffset(char const * bytes, size_t offset) {
	T value;
	bytes += offset;
	BytesToNumber(bytes, value);
	return value;
}

#endif // Utils_h
