#pragma once


//str: NULL-terminated string
inline bool str_contains(const char * str, char c) {
	while (str != NULL) {
		if (c == *str) {
			return true;
		}
		str++;
	}
	return false;
}