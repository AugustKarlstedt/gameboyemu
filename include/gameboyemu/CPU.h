#pragma once

#include <ostream> 

#include "gameboyemu/register.h"

#define FLAG_ZERO_BIT 7
#define FLAG_SUBTRACT_BIT 6
#define FLAG_HALF_CARRY_BIT 5
#define FLAG_CARRY_BIT 4


class CpuState
{
public:
	CpuState(uint8_t* memory, uint8_t* stack)
	{
		this->memory = memory;
		this->stack = stack;
	}

	CpuState(CpuState& oldState)
	{
		this->AF = oldState.AF;
		this->BC = oldState.BC;
		this->DE = oldState.DE;
		this->HL = oldState.HL;

		this->PC = oldState.PC;
		this->SP = oldState.SP;

		this->stack = oldState.stack;
		this->memory = oldState.memory;
	}

	Register AF;
	Register BC;
	Register DE;
	Register HL;

	Register PC;
	Register SP;

	uint8_t* stack;
	uint8_t* memory;

private:

};

class Command
{
public:
	virtual CpuState execute(CpuState cpu_state) = 0; // returns a new cpu_state with modifications made
	
	friend std::ostream& operator<<(std::ostream& os, const Command& c)
	{
		return os << c.debug_string;
	}

protected:
	std::string debug_string = "NOT IMPLEMENTED";
};
