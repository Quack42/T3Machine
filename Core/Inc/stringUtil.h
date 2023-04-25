#pragma once

//str: '\0' terminated string
inline bool str_contains(const char * str, char c) {
	while (*str != '\0') {
		if (c == *str) {
			return true;
		}
		str++;
	}
	return false;
}
