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

inline unsigned int uint_to_str(char * data, unsigned int maxLength, unsigned int uintToConvert) {
	unsigned int i=0;
	data[i] = '0'; 	//default value; to fix the "0 doesn't seem to have digits, mathematically" problem.

	// Compute the number of digits.
	unsigned int digits = 0;
	{
		int uintToConvertTemp = uintToConvert;
		while (uintToConvertTemp) {
			uintToConvertTemp /= 10;
			digits++;
		}
	}

	// Check if it fits in the target 'data' buffer.
	if (digits > maxLength) {
		// It doesn't fit. Write down X.
		data[i] = 'X';
		i++;
		return i;
	}

	// Convert the value to digits.
	if (digits == 0) {
		i++;
	} else {
		for (unsigned int i2=0; i2 < digits; i2++) {
			data[i+(digits-1)-i2] = '0' + uintToConvert % 10;
			uintToConvert /= 10;
		}
		i += digits;
	}

	return i;
}

inline unsigned int int_to_str(char * data, unsigned int maxLength, int intToConvert) {
	unsigned int i=0;
	data[i] = '0'; 	//default value; to fix the "0 doesn't seem to have digits, mathematically" problem.

	if (intToConvert < 0) {
		// Given number is negative.
		// Add a '-' to the data.
		data[i] = '-';
		i++;
		// Flip signedness to make sure remainder of the function handles it normally.
		intToConvert = -intToConvert;
	}

	return i + uint_to_str(&data[i], maxLength-i, intToConvert);
}

inline unsigned int float_to_str(char * data, unsigned int maxLength, float floatToConvert) {
	unsigned int i=0;
	data[i] = '0'; 	//default value; to fix the "0 doesn't seem to have digits, mathematically" problem.

	if (floatToConvert < 0) {
		// Given number is negative.
		// Add a '-' to the data.
		data[i] = '-';
		i++;
		// Flip signedness to make sure remainder of the function handles it normally.
		floatToConvert = -floatToConvert;
	}

	floatToConvert += 0.0005f; 	// For rounding, needs to be added early.

	// Convert whole numbers to int.
	i += uint_to_str(&data[i], maxLength, floatToConvert);

	// Convert remainder of float to an integer.
	// First get the remainder.
	float floatDecimals = floatToConvert - (int)floatToConvert;
	// Convert decimal numbers of interest into whole numbers.
	floatDecimals *= 1000.f;
	// Cut off remainder
	int intDecimals = floatDecimals;
	// Flip signedness of intDecimals; it's already handled at the start.
	if (intDecimals < 0) {
		intDecimals = -intDecimals;
	}

	// If there are decimals to convert, add a '.'
	if (i < maxLength) {
		data[i] = '.';
		i++;
	}

	// Count decimals; ignore lower '0' values.
	int digits = 3;
	while ((digits > 0) && (intDecimals % 10) == 0) {
		digits--;
		intDecimals /= 10;
	}
	
	// Add decimals to the buffer.
	if (digits + i < maxLength) {
		i += uint_to_str(&data[i], maxLength-i, intDecimals);
	}
	
	return i;
}
