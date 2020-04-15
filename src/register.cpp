#include "gameboyemu/register.h"
#include "..\include\gameboyemu\register.h"
#include <cassert>

Register::Register()
{
	value = 0;
}

void Register::setValue(uint16_t newValue)
{
	value = newValue;
}

void Register::setValue(uint8_t highByte, uint8_t lowByte)
{
	setHighByte(highByte);
	setLowByte(lowByte);
}

std::uint16_t Register::getValue()
{
	return value;
}

void Register::setLowByte(uint8_t lowByte)
{
	value &= 0xFF00;
	value |= lowByte;
}

std::uint8_t Register::getLowByte()
{
	return static_cast<uint8_t>(value & 0x00FF);
}

void Register::setHighByte(uint8_t highByte)
{
	value &= 0x00FF;
	value |= (highByte << 8);
}

std::uint8_t Register::getHighByte()
{
	return static_cast<uint8_t>((value & 0xFF00) >> 8);
}

void Register::setBit(uint8_t position)
{
	value |= (1 << position);
}

void Register::clearBit(uint8_t position)
{
	value &= ~(1 << position);
}

void Register::toggleBit(uint8_t position)
{
	value ^= (1 << position);
}

bool Register::checkBit(uint8_t position)
{
	return value & (1 << position);
}

bool Register::checkLowBit(uint8_t position)
{
	return getLowByte() & (1 << position);
}

bool Register::checkHighBit(uint8_t position)
{
	return getHighByte() & (1 << position);
}

void Register::add(uint16_t amount)
{
	value += amount;
}

void Register::addLowByte(uint8_t amount)
{
	setLowByte(getLowByte() + amount);
}

void Register::addHighByte(uint8_t amount)
{
	setHighByte(getHighByte() + amount);
}

void Register::subtract(uint16_t amount)
{
	value -= amount;
}

void Register::subtractLowByte(uint8_t amount)
{
	setLowByte(getLowByte() - amount);
}

void Register::subtractHighByte(uint8_t amount)
{
	setHighByte(getHighByte() - amount);
}

void Register::increment()
{
	value += 1;
}

void Register::incrementLowByte()
{
	setLowByte(getLowByte() + 1);
}

void Register::incrementHighByte()
{
	setHighByte(getHighByte() + 1);
}

void Register::decrement()
{
	value -= 1;
}

void Register::decrementLowByte()
{
	setLowByte(getLowByte() - 1);
}

void Register::decrementHighByte()
{
	setHighByte(getHighByte() - 1);
}

void Register::xor()
{
	value ^= value;
}

void Register::xorLowByte()
{
	setLowByte(getLowByte() ^ getLowByte());
}

void Register::xorHighByte()
{
	setHighByte(getHighByte() ^ getHighByte());
}

void Register::rotateLeft()
{
	value = value << 1;
}

void Register::rotateLowByteLeft()
{
	setLowByte(getLowByte() << 1);
}

void Register::rotateHighByteLeft()
{
	setHighByte(getHighByte() << 1);
}

