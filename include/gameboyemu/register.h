#pragma once

#include <iostream>

class Register
{
public:
	Register();

	void setValue(uint16_t newValue);
	void setValue(uint8_t highByte, uint8_t lowByte);
	uint16_t getValue();

	void setLowByte(uint8_t lowByte);
	uint8_t getLowByte();

	void setHighByte(uint8_t highByte);
	uint8_t getHighByte();

	void setBit(uint8_t position);
	void clearBit(uint8_t position);
	void toggleBit(uint8_t position);

	bool checkBit(uint8_t position);
	bool checkLowBit(uint8_t position);
	bool checkHighBit(uint8_t position);

	void add(uint16_t amount);
	void addLowByte(uint8_t amount);
	void addHighByte(uint8_t amount);

	void subtract(uint16_t amount);
	void subtractLowByte(uint8_t amount);
	void subtractHighByte(uint8_t amount);

	void increment();
	void incrementLowByte();
	void incrementHighByte();

	void decrement();
	void decrementLowByte();
	void decrementHighByte();

	void xor();
	void xorLowByte();
	void xorHighByte();

	void rotateLeft();
	void rotateLowByteLeft();
	void rotateHighByteLeft();

private:
	uint16_t value;

};