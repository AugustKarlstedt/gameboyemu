#include <iomanip>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>

#define MAX_FILE_SIZE 0x800000 // 8MB in bytes

int main()
{
    uint8_t* romStorage = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * MAX_FILE_SIZE));

    std::cout << "READING ROM\n";
    std::ifstream file;
    file.open("DMG_ROM.bin", std::ifstream::in);
    //file.open("Tetris (World) (Rev A).gb", std::ifstream::in);
    //file.open("Densha de Go! 2 (Japan).gbc", std::ifstream::in);
    uint32_t byte = 0;
    while (file.good())
    {
        if (byte > MAX_FILE_SIZE)
        {
            std::cout << "File too large.\n";
            return -1;
        }

        romStorage[byte] = file.get();
        ++byte;
    }
    byte -= 1;
    file.close();
    std::cout << "READING ROM COMPLETE (" << byte << " bytes)\n";
    
    uint8_t* stack = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * 0x10000));
    
    std::map<std::string, uint16_t> registers;
    registers["A"] = 0; //
    registers["B"] = 0; 
    registers["C"] = 0; //
    registers["D"] = 0;
    registers["E"] = 0;
    registers["F"] = 0;
    registers["H"] = 0; // 
    registers["L"] = 0; //
    registers["I"] = 0;
    registers["R"] = 0;

    registers["AF"] = 0;
    registers["BC"] = 0;
    registers["DE"] = 0;
    registers["HL"] = 0; //

    registers["PC"] = 0;
    registers["SP"] = 0; //

    registers["IX"] = 0;
    registers["IY"] = 0;

    registers["IXH"] = 0;
    registers["IXL"] = 0;
    registers["IYH"] = 0;
    registers["IYL"] = 0;

    std::cout << "PARSING ROM\n";
    for (int i = 0; i < byte; ++i)
    {
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(6) << i << ": ";

        if (i >= 0xA8 && i < 0xD8)
        {
            std::cout << "NINTENDO LOGO DATA\n";
            continue;
        }

        if (i >= 0xD8 && i < 0xE0)
        {
            std::cout << "REGISTERED TRADEMARK SYMBOL DATA\n";
            continue;
        }

        switch (romStorage[i])
        {
        case 0x31:
            std::cout << "LD SP,$" << std::hex << +romStorage[i + 2] << +romStorage[i + 1] << "\n";
            registers["SP"] = (romStorage[i + 2] << 8) + romStorage[i + 1];
            i += 2;
            break;
        case 0xAF:
            std::cout << "XOR A" << "\n";
            registers["A"] = registers["A"] ^ registers["A"];
            break;
        case 0x21:
            std::cout << "LD HL,$" << std::hex << std::setfill('0') << std::setw(2) << +romStorage[i + 2] << std::setw(2) << +romStorage[i + 1] << "\n";
            i += 2;
            break;
        case 0x32:
            std::cout << "LD (HL-),A\n";
            break;
        case 0xCB:
            switch (romStorage[i + 1])
            {
            case 0x7C:
                std::cout << "BIT 7,H\n";
                break;
            case 0x11:
                std::cout << "RL C\n";
                break;
            default:
                std::cout << "UNKNOWN WITH CB PREFIX: " << std::hex << +romStorage[i + 1] << "\n";
                break;
            }
            i += 1;
            break;
        case 0x20:
            if (romStorage[i + 1] >= 0xFE)
            {
                std::cout << "JR NZ,$" << +romStorage[i + 1] << "\n";
            }
            else
            {
                std::cout << "JR NZ, Addr_" << std::uppercase << std::setfill('0') << std::setw(4) << static_cast<char>(i) + 2 + romStorage[i + 1] << std::nouppercase << "\n";
            }
            i += 1;
            break;
        case 0x0E:
            std::cout << "LD C,$" << std::setfill('0') << std::setw(2) << +romStorage[i + 1] << "\n";
            i += 1;
            break;
        case 0x3E:
            std::cout << "LD A,$" << std::setfill('0') << std::setw(2) << +romStorage[i + 1] << "\n";
            i += 1;
            break;
        case 0xE2:
            std::cout << "LD ($FF00+C),A" << "\n";
            break;
        case 0x0C:
            std::cout << "INC C" << "\n";
            break;
        case 0x77:
            std::cout << "LD (HL),A\n";
            break;
        case 0xE0:
            std::cout << "LD ($FF00+$" << +romStorage[i + 1] << "),A" << "\n";
            i += 1;
            break;
        case 0x11:
            std::cout << "LD DE,$" << std::setfill('0') << std::setw(2) << +romStorage[i + 2] << std::setw(2) << +romStorage[i + 1] << "\n";
            i += 2;
            break;
        case 0x1A:
            std::cout << "LD A,(DE)\n";
            break;
        case 0xCD:
            std::cout << "CALL $" << std::setfill('0') << std::setw(2) << +romStorage[i + 2] << std::setw(2) << +romStorage[i + 1] << "\n";
            i += 2;
            break;
        case 0x13:
            std::cout << "INC DE\n";
            break;
        case 0x7B:
            std::cout << "LD A,E\n";
            break;
        case 0xFE:
            std::cout << "CP $" << +romStorage[i + 1] << "\n";
            i += 1;
            break;
        case 0x06:
            std::cout << "LD B,$" << std::setfill('0') << std::setw(2) << +romStorage[i + 1] << "\n";
            i += 1;
            break;
        case 0x22:
            std::cout << "LD (HL+),A\n";
            break;
        case 0x23:
            std::cout << "INC HL\n";
            break;
        case 0x05:
            std::cout << "DEC B\n";
            break;
        case 0xEA:
            std::cout << "LD ($" << +romStorage[i + 2] << +romStorage[i + 1] << ",A\n";
            i += 2;
            break;
        case 0x3D:
            std::cout << "DEC A\n";
            break;
        case 0x28:
            std::cout << "JR Z, Addr_" << +romStorage[i + 1] << "\n";
            i += 1;
            break;
        case 0x0D:
            std::cout << "DEC C\n";
            break;
        case 0x2E:
            std::cout << "LD L,$" << std::setfill('0') << std::setw(2) << +romStorage[i + 1] << "\n";
            i += 1;
            break;
        case 0x18:
            std::cout << "JR Addr_" << std::setfill('0') << std::setw(2) << +romStorage[i + 1] << "\n";
            i += 1;
            break;
        case 0x67:
            std::cout << "LD H,A\n";
            break;
        case 0x57:
            std::cout << "LD D,A\n";
            break;
        case 0x04:
            std::cout << "INC B\n";
            break;
        case 0x1E:
            std::cout << "LD E,$" << std::setfill('0') << std::setw(2) << +romStorage[i + 1] << "\n";
            i += 1;
            break;
        case 0xF0:
            std::cout << "LD A,($FF00+$" << +romStorage[i + 1] << ")\n";
            i += 1;
            break;
        case 0x1D:
            std::cout << "DEC E\n";
            break;
        case 0x24:
            std::cout << "INC H\n";
            break;
        case 0x7C:
            std::cout << "LD A,H\n";
            break;
        case 0x90:
            std::cout << "SUB B\n";
            break;
        case 0x15:
            std::cout << "DEC D\n";
            break;
        case 0x16:
            std::cout << "LD D,$" << std::setfill('0') << std::setw(2) << +romStorage[i + 1] << "\n";
            i += 1;
            break;
        case 0x4F:
            std::cout << "LD C,A\n";
            break;
        case 0xC5: 
            std::cout << "PUSH BC\n";
            break;
        case 0x17:
            std::cout << "RLA\n";
            break;
        case 0xC1:
            std::cout << "POP BC\n";
            break;
        case 0xC9:
            std::cout << "RET\n";
            break;
        case 0xBE:
            std::cout << "CP (HL)\n";
            break;
        case 0x7D:
            std::cout << "LD A,L\n";
            break;
        case 0x78:
            std::cout << "LD A,B\n";
            break;
        case 0x86:
            std::cout << "ADD (HL)\n";
            break;
        default:
            std::cout << "UNKNOWN: " << std::hex << +romStorage[i] << "\n";
            break;
        }
    }
    std::cout << "PARSING ROM COMPLETE\n";

    return 0;
}