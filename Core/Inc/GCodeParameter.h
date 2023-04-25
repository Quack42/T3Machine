#pragma once

class GCodeParameter {
private:
	char identifier;
	float value;
public:
	GCodeParameter() :
			identifier('\0')
	{
	}

	GCodeParameter(char identifier, float value) :
			identifier(identifier),
			value(value)
	{

	}
	// void setIdentifier(char identifier) {
	// 	this->identifier = identifier;
	// }

	// void setValue(float value) {
	// 	this->value = value;
	// }

	char getIdentifier() const {
		return identifier;
	}

	float getValue() const {
		return value;
	}
};

