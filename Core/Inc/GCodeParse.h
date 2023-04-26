#pragma once

#include "GCodeCommand.h"
#include "GCodeParameter.h"
#include "stringUtil.h"

#include <cstdint>
#include <cmath>

class GCodeParse {
private:
	const char * dataToParse;
	uint32_t dataToParseLength;

	struct String_s {
		const char * data = nullptr;
		unsigned int dataLength = 0;
	};

	String_s gCodeCodeString;
	String_s parameterStringList[kMaximumNumberOfParameters];
	unsigned int parameterStringListLength = 0;

public:
	GCodeParse(const char * dataToParse, uint32_t dataToParseLength) :
			dataToParse(dataToParse),
			dataToParseLength(dataToParseLength)
	{
	}

	bool parse() {
		// Return false on invalid structure.
		parameterStringListLength = 0;

		skipWhiteSpaceCharacters();

		if (!parseGCodeCode()){
			return false;
		}

		if (!parseParameters()) {
			return false;
		}

		return true;
	}

	bool generateCommand(GCodeCommand * command) {
		if (command == nullptr) {
			return false;
		}

		if (gCodeCodeString.data[0] == 'G') {
			if (!convertGCodeCode(command)) {
				return false;
			}
		}

		if (!convertParameters(command)) {
			return false;
		}

		return true;
	}

private:

	bool convertGCodeCode(GCodeCommand * command) {
		const char kNumbers[11] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0'};

		unsigned int gValue = 0;
		for (unsigned int i=0; i < gCodeCodeString.dataLength-1; i++) {
			if (!str_contains(kNumbers, gCodeCodeString.data[1+i])) {
				// Data at this point must be a number. This is not a number so return false, indicating failure.
				return false;
			}
			// Shift everything up 1 digit.
			gValue *= 10;
			// Add new number as lowest digit (reading from left to right).
			gValue += (gCodeCodeString.data[1+i] - '0');
		}

		command->setCode(static_cast<GCode_e>(e_GBase + gValue));
		return true;
	}

	bool convertParameters(GCodeCommand * command) {
		const char kNumbers[11] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0'};
		const char kDecimalPoint[2] = {'.', '\0'};

		for (unsigned int i=0; i < parameterStringListLength; i++) {
			String_s & parameterString = parameterStringList[i];

			// First read the identifier.
			const char parameterIdentifier = parameterString.data[0];

			bool negativeNumber = false;
			if (parameterString.dataLength > 1 && parameterString.data[1] == '-') {
				negativeNumber = true;
			}

			// Then read the parameter value.
			float parameterValue = 0.0f;
			bool decimalPointFound = false;
			unsigned int digitsSinceDecimalPointFound = 0;
			for (unsigned int i2=negativeNumber ? 2 : 1; i2 < parameterString.dataLength; i2++) {
				const char & parameterDigit = parameterString.data[i2];
				if (str_contains(kNumbers, parameterDigit)) {
					if (!decimalPointFound) {
						// Treat this number as if the decimal point has NOT been encountered yet.
						// Shift everything up 1 digit.
						parameterValue *= 10.0f;
						// Add new number as lowest digit (reading from left to right).
						parameterValue += (parameterDigit - '0');
					} else {
						// Treat this number as if the decimal point HAS been encountered.
						// Decimal point has been found; treat it as such.
						float digitValue = (parameterDigit - '0');
						// Shift the digit value to the appropriate digit (i.e. divide it by 10 by the amount of digits that have been found after the decimal point).
						digitValue *= std::pow(0.1f, (digitsSinceDecimalPointFound+1));

						// Add digit value to parameter value.
						parameterValue += digitValue;

						// Increment number of digits encountered since decimal point has been found.
						digitsSinceDecimalPointFound++;
					}
				} else if (str_contains(kDecimalPoint, parameterDigit)) {
					if (decimalPointFound) {
						// Found a second decimal point, parameter is faulty.
						return false;
					}
					// For further processing: remember that the decimal point has been found.
					decimalPointFound = true;
					digitsSinceDecimalPointFound = 0;
				} else {
					// Character found that was not a digit, nor a decimal point.
					return false;
				}
			}

			// Switch sign if there was an indication of a negative number
			if (negativeNumber) {
				parameterValue = -parameterValue;
			}

			// Store the parameter in the command.
			if (!command->addParameter(GCodeParameter(parameterIdentifier, parameterValue))) {
				return false;
			}
		}

		return true;
	}

	//str: NULL-terminated string
	bool skip(const char * str) {
		while ((dataToParseLength > 0) && str_contains(str, *dataToParse)) {
			dataToParse++;
			dataToParseLength--;
		}

		if (dataToParseLength == 0) {
			return false;
		}
		return true;
	}


	//str: NULL-terminated string
	bool skipUntil(const char * str) {
		while ((dataToParseLength > 0) && !str_contains(str, *dataToParse)) {
			dataToParse++;
			dataToParseLength--;
		}

		if (dataToParseLength == 0) {
			return false;
		}
		return true;
	}

	bool skipWhiteSpaceCharacters() {
		const char kWhiteSpaceCharacters[3] = {' ', '\t', '\0'};
		return skip(kWhiteSpaceCharacters);
	}

	bool skipUntilWhiteSpaceCharacters() {
		const char kWhiteSpaceCharacters[3] = {' ', '\t', '\0'};
		return skipUntil(kWhiteSpaceCharacters); 	//removed '\r' and '\n' from this list as they have been used to subdivide 'dataToParse' prior to this (and are no longer included in 'data')
	}

	bool parseGCodeCode() {
		const char kGCodeChar[2] = {'G', '\0'};
		if (str_contains(kGCodeChar, *dataToParse)) {
			// Store start of GCode string.
			gCodeCodeString.data = dataToParse;
			// Skip until we find a white space character (or until the string ends), marking the end of the GCode string.
			skipUntilWhiteSpaceCharacters();
			// Store end of GCode string.
			gCodeCodeString.dataLength = (dataToParse - gCodeCodeString.data);
			// Skip white space characters until either we find a new character (a parameter) or until the string ends.
			skipWhiteSpaceCharacters();
		} else {
			return false;
		}
		return true;
	}

	bool parseParameters() {
		while ((dataToParseLength > 0) && (parameterStringListLength < kMaximumNumberOfParameters)) {
			// Keep parsing if there is something left to parse.
			// By arrangement: 'dataToParse' points to either a non-whitespace-character, or 'dataToParseLength' is 0.
			parameterStringList[parameterStringListLength].data = dataToParse;
			// Skip until we find a white space character (or until the string ends), marking the end of the parameter.
			skipUntilWhiteSpaceCharacters();
			// Store end of parameter string.
			parameterStringList[parameterStringListLength].dataLength = (dataToParse - parameterStringList[parameterStringListLength].data);
			// Indicate the number of parameters have grown by 1.
			parameterStringListLength++;
			
			// Make ready to parse next parameter by skipping white spaces until either
			// - the end of the string (i.e. dataToParseLength==0)
			// - or until we find the next character.
			skipWhiteSpaceCharacters();
		}

		if ((dataToParseLength > 0) && (parameterStringListLength == kMaximumNumberOfParameters)) {
			//Not enough spaces to store parameter data.
			//TODO: Let developer know this has occurred.
			return false;
		}

		return true;
	}
};
