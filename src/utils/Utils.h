#ifndef Utils_h
#define Utils_h

#include <cstddef>

// Playing with fire here!

template<typename T>
inline void BytesToNumber(char const * bytes, T & number) {
	number = *(reinterpret_cast<T const *>(bytes));
}

template<typename T>
inline void NumberToBytes(T number, char * bytes) {
	char const * numberPtr = reinterpret_cast<char const *>(&number);
	for (size_t i = 0; i < sizeof(T); ++i)
		bytes[i] = numberPtr[i];
}

#endif // Utils_h
