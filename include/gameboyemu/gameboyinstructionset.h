#pragma once

#include <initializer_list>
#include <map>
#include <string>
#include "gameboyemu/CPU.h"
#include "gameboyemu/register.h"

// todo:
// create a baseinstructionset.h file that has some
// defines and helpers in it
// maybe #define little_endian big_endian
// and each instruction set that is derived from it
// has to define some config parameters
//

class ldsp : public Command
{
public:
	ldsp()
	{
		this->debug_string = "LD SP, $abcd";
	}

	CpuState execute(CpuState cpu_state)
	{
		CpuState newState(cpu_state);
		newState.SP.setValue(cpu_state.memory[cpu_state.PC.getValue() + 2], 
							 cpu_state.memory[cpu_state.PC.getValue() + 1]);
		newState.PC.setValue(cpu_state.PC.getValue() + 3);
		return newState;
	}
};

class xora : public Command
{
public:
	xora()
	{
		this->debug_string = "XOR A";
	}

	CpuState execute(CpuState cpu_state)
	{
		CpuState newState(cpu_state);
		newState.SP.setValue(cpu_state.memory[cpu_state.PC.getValue() + 2],
			cpu_state.memory[cpu_state.PC.getValue() + 1]);
		newState.PC.setValue(cpu_state.PC.getValue() + 1);
		return newState;
	}
};

std::map<uint8_t, Command*> instructions = {
	{0x31, new ldsp()},
	{0xAF, new xora()},
	//{0x21, Command(3)},
	//{0x32, Command(1)},
	//{0xCB, Command(2)},
	//{0x20, Command(2)},
	//{0x0E, Command(2)},
	//{0x3E, Command(2)},
	//{0xE2, Command(1)},
	//{0x0C, Command(1)},
	//{0x77, Command(1)},
	//{0xE0, Command(2)},
	//{0x11, Command(3)},
	//{0x1A, Command(1)},
	//{0xCD, Command(3)},
	//{0x13, Command(1)},
	//{0x7B, Command(1)},
	//{0xFE, Command(2)},
	//{0x06, Command(2)},
	//{0x22, Command(1)},
	//{0x23, Command(1)},
	//{0x05, Command(1)},
	//{0xEA, Command(3)},
	//{0x3D, Command(1)},
	//{0x28, Command(2)},
	//{0x0D, Command(1)},
	//{0x2E, Command(2)},
	//{0x18, Command(2)},
	//{0x67, Command(1)},
	//{0x57, Command(1)},
	//{0x04, Command(1)},
	//{0x1E, Command(2)},
	//{0xF0, Command(2)},
	//{0x1D, Command(1)},
	//{0x24, Command(1)},
	//{0x7C, Command(1)},
	//{0x90, Command(1)},
	//{0x15, Command(1)},
	//{0x16, Command(2)},
	//{0x4F, Command(1)},
	//{0xC5, Command(1)},
	//{0x17, Command(1)},
	//{0xC1, Command(1)},
	//{0xC9, Command(1)},
	//{0xBE, Command(1)},
	//{0x7D, Command(1)},
	//{0x78, Command(1)},
	//{0x86, Command(1)},
	//{0x00, Command(1)},
	//{0x3c, Command(1)}
};

std::map<uint8_t, Command*> instructions_cb_prefix = {
	//{0x7C, Command(1)},
	//{0x11, Command(1)}
};