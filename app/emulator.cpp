#include <iomanip>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>

//#define USE_FMT
#ifdef USE_FMT
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/compile.h>
#else // fake fmt
int fg(...) { return 0; };
namespace fmt
{
    void print(...) {};
    namespace color
    {
        int lime = 0;
        int red = 0;
    }
    namespace emphasis
    {
        int bold = 0;
    }
}
#endif

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "gameboyemu/BuildSettings.h"
#include "gameboyemu/CPU.h"
#include "gameboyemu/register.h"


#define MAX_FILE_SIZE 0x800000 // 8MB in bytes

const int16_t WAVE_DUTY_00[] = { 0, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 0, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 0, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767 };
const int16_t WAVE_DUTY_01[] = { 0, 0, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 0, 0, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 0, 0, 32767, 32767, 32767, 32767, 32767, 32767, 32767 };
const int16_t WAVE_DUTY_10[] = { 0, 0, 0, 0, 32767, 32767, 32767, 32767, 32767, 0, 0, 0, 0, 32767, 32767, 32767, 32767, 32767, 0, 0, 0, 0, 32767, 32767, 32767, 32767, 32767 };
const int16_t WAVE_DUTY_11[] = { 0, 0, 0, 0, 0, 0, 32767, 32767, 32767, 0, 0, 0, 0, 0, 0, 32767, 32767, 32767, 0, 0, 0, 0, 0, 0, 32767, 32767, 32767 };

int main()
{
    fmt::print(fg(fmt::color::lime) | fmt::emphasis::bold, "Version {}.{}\n", VERSION_MAJOR, VERSION_MINOR);

    uint8_t* stack = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * 0xFFFF));
    uint8_t* memory = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * 0xFFFF));
    
    fmt::print("READING BIOS\n");
    std::ifstream bios;
    bios.open("DMG_ROM.bin", std::ifstream::in | std::ifstream::binary);
    uint32_t bios_byte = 0;
    while (bios.good())
    {
        if (bios_byte > MAX_FILE_SIZE)
        {
            fmt::print("File too large.\n");
            return -1;
        }

        memory[bios_byte] = bios.get();
        ++bios_byte;
    }
    bios_byte -= 1;
    bios.close();
    fmt::print("READING BIOS COMPLETE ({} bytes)\n", bios_byte);

    fmt::print("READING ROM\n");
    std::ifstream rom;
    rom.open("Tetris (World) (Rev A).gb", std::ifstream::in | std::ifstream::binary);
    uint32_t rom_byte = bios_byte;
    while (rom.good())
    {
        if (bios_byte > 0) {
            rom.get();
            --bios_byte;
            continue;
        }

        if (rom_byte > MAX_FILE_SIZE)
        {
            fmt::print("File too large.\n");
            return -1;
        }

        memory[rom_byte] = rom.get();
        ++rom_byte;
    }
    rom_byte -= 1;
    rom_byte -= bios_byte;
    rom.close();
    fmt::print("READING ROM COMPLETE ({} bytes)\n", rom_byte);

    CpuState cpu_state(memory, stack);

    //
    sf::RenderWindow window(sf::VideoMode(160, 144), "gameboyemu");
    //sf::RenderWindow window(sf::VideoMode(256, 256), "gameboyemu");

    sf::Texture texture;
    texture.create(256, 256);
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sf::Uint8* pixels = new sf::Uint8[256 * 256 * 4]; // RGBA
    sf::RectangleShape windowSprite(sf::Vector2f(160, 144));
    windowSprite.setFillColor(sf::Color::Transparent);
    windowSprite.setOutlineThickness(2);
    windowSprite.setOutlineColor(sf::Color::Red);

    sf::SoundBuffer soundBuffer;
    soundBuffer.loadFromSamples(WAVE_DUTY_00, 27, 1, 1024);

    sf::Sound sound;
    sound.setBuffer(soundBuffer);

    //sound.setLoop(true);
    //

    bool cont = true;
    bool step = false;

    // todo: find out where this gets initialized in actual hardware
    // prob just set to zero somewhere
    cpu_state.memory[0xFF42] = 0;
    cpu_state.memory[0xFF43] = 0;

    fmt::print("PARSING ROM\n");
    while(window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    return -1;
                    break;
                case sf::Keyboard::C:
                    cont = !cont;
                    break;
                case sf::Keyboard::S:
                    step = true;
                    break;
                default:
                    // only works for a-z idk why whatever
                    std::cout << "Key pressed: " << (char)(event.key.code+97) << "\n";
                    break;
                }
                break;
            default:
                break;
            }
        }

        uint8_t counter = 0;
        //for (int i = 0; i < 0x2000; i += 16)
        //{
        //    if (i != 0 && i % 0x200 == 0) ++counter;
        //    for (int j = 0; j < 16; j += 2)
        //    {
        //        uint8_t byte1 = memory[0x8000 + i + j];
        //        uint8_t byte2 = memory[0x8000 + i + j + 1];
        //        uint8_t line = byte1 | byte2;

        //        for (int k = 0; k < 8; ++k)
        //        {
        //            uint8_t bit = 7 - k;
        //            uint8_t color_value = 0;
        //            color_value |= ((byte1 & (1 << bit)) >> bit) << 1;
        //            color_value |= ((byte2 & (1 << bit)) >> bit);

        //            uint8_t color = 0;
        //            switch (color_value)
        //            {
        //            case 0:
        //                color = (cpu_state.memory[0xFF47] & 0b00000011);
        //                color = 0;
        //                break;
        //            case 1:
        //                color = (cpu_state.memory[0xFF47] & 0b00001100) >> 2;
        //                color = 128;
        //                break;
        //            case 2:
        //                color = (cpu_state.memory[0xFF47] & 0b00110000) >> 4;
        //                color = 200;
        //                break;
        //            case 3:
        //                color = (cpu_state.memory[0xFF47] & 0b11000000) >> 6;
        //                color = 255;
        //                break;
        //            default:
        //                fmt::print("WTF?\n");
        //                break;
        //            }

        //            uint32_t row = (j / 2) * 256 * 4; // j = 0..7
        //            uint32_t col = (i / 2) * 4 + ((8-1)*counter * 256*4); // i = 0..256
        //            uint32_t pixel = k * 4;
        //            pixels[col + row + pixel + 0] = color;
        //            pixels[col + row + pixel + 1] = color;
        //            pixels[col + row + pixel + 2] = color;
        //            pixels[col + row + pixel + 3] = 255;
        //        }
        //    }
        //}

        counter = 0;
        for (int i = 0; i < 0x0400; ++i)
        {
            if (i != 0 && i % 32 == 0) ++counter;

            uint8_t index = cpu_state.memory[0x9800 + i];

            for (int j = 0; j < 16; j += 2)
            {
                uint8_t byte1 = cpu_state.memory[0x8000 + (index * 16) + j];
                uint8_t byte2 = cpu_state.memory[0x8000 + (index * 16) + j + 1];

                for (int k = 0; k < 8; ++k)
                {
                    uint8_t bit = 7 - k;
                    uint8_t color_value = 0;
                    color_value |= ((byte1 & (1 << bit)) >> bit) << 1;
                    color_value |= ((byte2 & (1 << bit)) >> bit);

                    uint8_t color = 0;
                    switch (color_value)
                    {
                    case 0:
                        color = (cpu_state.memory[0xFF47] & 0b00000011);
                        color *= 85;
                        break;
                    case 1:
                        color = (cpu_state.memory[0xFF47] & 0b00001100) >> 2;
                        color *= 85;
                        break;
                    case 2:
                        color = (cpu_state.memory[0xFF47] & 0b00110000) >> 4;
                        color *= 85;
                        break;
                    case 3:
                        color = (cpu_state.memory[0xFF47] & 0b11000000) >> 6;
                        color *= 85;
                        break;
                    default:
                        fmt::print("WTF?\n");
                        break;
                    }

                    uint32_t row = ((j / 2) * 256 * 4) + (counter * 256 * 4 * (8-1)); // j = 0..7
                    uint32_t col = (i * 4 * 8); // i = 0..1024
                    uint32_t pixel = k * 4;
                    pixels[col + row + pixel + 0] = color;
                    pixels[col + row + pixel + 1] = color;
                    pixels[col + row + pixel + 2] = color;
                    pixels[col + row + pixel + 3] = 200;
                }
            }
        }

        window.clear(sf::Color(17, 17, 17));
        texture.update(pixels);
        window.draw(sprite);

        windowSprite.setPosition(cpu_state.memory[0xFF43], cpu_state.memory[0xFF42]);
        window.draw(windowSprite);

        window.display();

        sf::View view = window.getDefaultView();
        view.reset(sf::FloatRect(cpu_state.memory[0xFF43], cpu_state.memory[0xFF42], 160, 144));
        window.setView(view);

        if (!cont) {
            if (!step) {
                continue;
            }
            else {
                step = false;
            }
        }

        // hack
        cpu_state.memory[0xFF44] = 144;

        switch ((cpu_state.memory[0xFF11] & 0b11000000) >> 6)
        {
        case 0b00:
            soundBuffer.loadFromSamples(WAVE_DUTY_00, 27, 1, (64 - t1) * (1 / 256));
            //soundBuffer.loadFromSamples(WAVE_DUTY_00, 27, 1, 64);
            break;
        case 0b01:
            soundBuffer.loadFromSamples(WAVE_DUTY_00, 27, 1, 27*500);
            //soundBuffer.loadFromSamples(WAVE_DUTY_01, 27, 1, 27*1000);
            break;
        case 0b10:
            soundBuffer.loadFromSamples(WAVE_DUTY_00, 27, 1, 27*250);
            //soundBuffer.loadFromSamples(WAVE_DUTY_10, 27, 1, 44100);
            break;
        case 0b11:
            soundBuffer.loadFromSamples(WAVE_DUTY_00, 27, 1, 27*100);
            //soundBuffer.loadFromSamples(WAVE_DUTY_11, 27, 1, 1024);
            break;
        default:
            break;
        }
        sound.setBuffer(soundBuffer);

        

        uint8_t instruction = cpu_state.memory[cpu_state.PC.getValue()];
        fmt::print("${:#04x} {:#02x}\n", cpu_state.PC.getValue(), instruction);

        fmt::print("-------- REGISTERS --------\n");
        fmt::print("AF: {:02x} {:08b}\n", cpu_state.AF.getHighByte(), cpu_state.AF.getLowByte());
        fmt::print("BC: {:02x} {:02x}\n", cpu_state.BC.getHighByte(), cpu_state.BC.getLowByte());
        fmt::print("DE: {:02x} {:02x}\n", cpu_state.DE.getHighByte(), cpu_state.DE.getLowByte());
        fmt::print("HL: {:02x} {:02x}\n", cpu_state.HL.getHighByte(), cpu_state.HL.getLowByte());
        fmt::print("PC: {:02x} {:02x}\n", cpu_state.PC.getHighByte(), cpu_state.PC.getLowByte());
        fmt::print("SP: {:02x} {:02x}\n", cpu_state.SP.getHighByte(), cpu_state.SP.getLowByte());
        
        fmt::print("-------- STACK --------\n");
        for (uint8_t i = 0; i < 8; ++i)
        {
            fmt::print("{:04x} {:02x}\n", 0xFFFE - i, cpu_state.stack[0xFFFE - i]);
        }

        switch (instruction)
        {
        case 0x31:
            cpu_state.SP.setValue(cpu_state.memory[cpu_state.PC.getValue() + 2], cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.PC.add(3);
            fmt::print("LD SP,${:04x}\n", cpu_state.SP.getValue());
            break;
        case 0xAF:
            cpu_state.AF.xorHighByte();
            if (cpu_state.AF.getHighByte() == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.PC.add(1);
            fmt::print("XOR A\n");
            break;
        case 0x21:
            cpu_state.HL.setValue(cpu_state.memory[cpu_state.PC.getValue() + 2], cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.PC.add(3);
            fmt::print("LD HL,${:04x}\n", cpu_state.HL.getValue());
            break;
        case 0x32:
            cpu_state.memory[cpu_state.HL.getValue()] = cpu_state.AF.getHighByte();
            cpu_state.HL.decrement();
            cpu_state.PC.add(1);
            fmt::print("LD (HL-),A\n");
            break;
        case 0xCB:
            switch (cpu_state.memory[cpu_state.PC.getValue() + 1])
            {
            case 0x7C:
            {
                if (cpu_state.HL.checkHighBit(7))
                {
                    cpu_state.AF.clearBit(FLAG_ZERO_BIT);
                }
                else
                {
                    cpu_state.AF.setBit(FLAG_ZERO_BIT);
                }
                cpu_state.AF.clearBit(FLAG_SUBTRACT_BIT);
                cpu_state.AF.setBit(FLAG_HALF_CARRY_BIT);
                fmt::print("BIT 7,H\n");
                break;
            }
            case 0x11:
            {
                bool hasCarryBitSet = cpu_state.AF.checkBit(FLAG_CARRY_BIT);
                if (cpu_state.BC.checkLowBit(7))
                {
                    cpu_state.AF.setBit(FLAG_CARRY_BIT);
                }
                else
                {
                    cpu_state.AF.clearBit(FLAG_CARRY_BIT);
                }

                cpu_state.BC.rotateLowByteLeft();
                if (hasCarryBitSet) cpu_state.BC.incrementLowByte(); // wtf

                if (cpu_state.BC.getLowByte() == 0)
                {
                    cpu_state.AF.setBit(FLAG_ZERO_BIT);
                }
                else
                {
                    cpu_state.AF.clearBit(FLAG_ZERO_BIT);
                }
                cpu_state.AF.clearBit(FLAG_SUBTRACT_BIT);
                cpu_state.AF.clearBit(FLAG_HALF_CARRY_BIT);
                fmt::print("RL C\n");
            }
                break;
            default:
                fmt::print("UNKNOWN WITH CB PREFIX: {:#02x}\n", cpu_state.memory[cpu_state.PC.getValue() + 1]);
                break;
            }
            cpu_state.PC.add(2);
            break;
        case 0x20:
        {
            uint16_t newAddr = cpu_state.PC.getValue() + 2 + static_cast<int8_t>(cpu_state.memory[cpu_state.PC.getValue() + 1]);
            if (!cpu_state.AF.checkLowBit(FLAG_ZERO_BIT))
            {
                cpu_state.PC.setValue(newAddr);
            }
            else
            {
                cpu_state.PC.add(2);
            }
            fmt::print("JR NZ, Addr_{:04x}\n", newAddr);
        }
            break;
        case 0x0E:
            fmt::print("LD C,${:#02x}\n", cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.BC.setLowByte(cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.PC.add(2);
            break;
        case 0x3E:
            fmt::print("LD A,${:#02x}\n", cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.AF.setHighByte(cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.PC.add(2);
            break;
        case 0xE2:
            fmt::print("LD ($FF00+C),A\n");
            cpu_state.memory[0xFF00 + cpu_state.BC.getLowByte()] = cpu_state.AF.getHighByte();
            cpu_state.PC.add(1);
            break;
        case 0x0C:
            fmt::print("INC C\n");
            if (cpu_state.BC.checkLowBit(3))
            {
                cpu_state.AF.setBit(FLAG_HALF_CARRY_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_HALF_CARRY_BIT);
            }
            cpu_state.BC.incrementLowByte();
            if (cpu_state.BC.getLowByte() == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.clearBit(FLAG_SUBTRACT_BIT);
            cpu_state.PC.add(1);
            break;
        case 0x77:
            fmt::print("LD (HL),A\n");
            cpu_state.memory[cpu_state.HL.getValue()] = cpu_state.AF.getHighByte();
            cpu_state.PC.add(1);
            break;
        case 0xE0:
            fmt::print("LD ($FF00+${:#02x}),A\n", cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.memory[0xFF00 + cpu_state.memory[cpu_state.PC.getValue() + 1]] = cpu_state.AF.getHighByte();
            cpu_state.PC.add(2);
            break;
        case 0x11:
            cpu_state.DE.setValue(cpu_state.memory[cpu_state.PC.getValue() + 2], cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.PC.add(3);
            fmt::print("LD DE,${:#04x}\n", cpu_state.DE.getValue());
            break;
        case 0x1A:
            fmt::print("LD A,(DE)\n");
            cpu_state.AF.setHighByte(cpu_state.memory[cpu_state.DE.getValue()]);
            cpu_state.PC.add(1);
            break;
        case 0xCD:
        {

            Register tmp;
            tmp.setValue(cpu_state.PC.getValue() + 3);

            stack[cpu_state.SP.getValue()] = tmp.getHighByte();
            cpu_state.SP.subtract(1);
            stack[cpu_state.SP.getValue()] = tmp.getLowByte();
            cpu_state.SP.subtract(1);

            cpu_state.PC.setValue(cpu_state.memory[cpu_state.PC.getValue() + 2], cpu_state.memory[cpu_state.PC.getValue() + 1]);

            fmt::print("CALL ${:04x}\n", cpu_state.PC.getValue());
            break;
        }
        case 0x13:
            fmt::print("INC DE\n");
            cpu_state.DE.increment();
            cpu_state.PC.add(1);
            break;
        case 0x7B:
            fmt::print("LD A,E\n");
            cpu_state.AF.setHighByte(cpu_state.DE.getLowByte());
            cpu_state.PC.add(1);
            break;
        case 0xFE:
        {
            fmt::print("CP ${:#02x}\n", cpu_state.memory[cpu_state.PC.getValue() + 1]);
            int8_t difference = cpu_state.AF.getHighByte() - cpu_state.memory[cpu_state.PC.getValue() + 1];
            if (difference == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.setBit(FLAG_SUBTRACT_BIT);
            //TODO: half carry
            if (difference < 0)
            {
                cpu_state.AF.setBit(FLAG_CARRY_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_CARRY_BIT);
            }
            cpu_state.PC.add(2);
            break;
        }
        case 0x06:
            fmt::print("LD B,${:02x}\n", cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.BC.setHighByte(cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.PC.add(2);
            break;
        case 0x22:
            fmt::print("LD (HL+),A\n");
            cpu_state.memory[cpu_state.HL.getValue()] = cpu_state.AF.getHighByte();
            cpu_state.HL.increment();
            cpu_state.PC.add(1);
            break;
        case 0x23:
            fmt::print("INC HL\n");
            cpu_state.HL.increment();
            cpu_state.PC.add(1);
            break;
        case 0x05:
            fmt::print("DEC B\n");
            cpu_state.BC.decrementHighByte();

            if (cpu_state.BC.getHighByte() == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.setBit(FLAG_SUBTRACT_BIT);
            //TODO: half carry

            cpu_state.PC.add(1);
            break;
        case 0xEA:
        {
            Register tmp;
            tmp.setValue(cpu_state.memory[cpu_state.PC.getValue() + 2], cpu_state.memory[cpu_state.PC.getValue() + 1]);
            fmt::print("LD (${:#04x}),A\n", tmp.getValue());
            cpu_state.memory[tmp.getValue()] = cpu_state.AF.getHighByte();
            cpu_state.PC.add(3);
        }
            break;
        case 0x3D:
            fmt::print("DEC A\n");
            cpu_state.AF.decrementHighByte();

            if (cpu_state.AF.getHighByte() == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.setBit(FLAG_SUBTRACT_BIT);
            // todo: half carry

            cpu_state.PC.add(1);
            break;
        case 0x28:
        {
            uint16_t newAddr = cpu_state.PC.getValue() + 2 + static_cast<int8_t>(cpu_state.memory[cpu_state.PC.getValue() + 1]);
            fmt::print("JR Z, Addr_{:04x}\n", newAddr);

            if (cpu_state.AF.checkLowBit(FLAG_ZERO_BIT))
            {
                cpu_state.PC.setValue(newAddr);
            }
            else
            {
                cpu_state.PC.add(2);
            }
            
            break;
        }
        case 0x0D:
            fmt::print("DEC C\n");
            cpu_state.BC.decrementLowByte();

            if (cpu_state.BC.getLowByte() == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.setBit(FLAG_SUBTRACT_BIT);
            // todo: half carry

            cpu_state.PC.add(1);
            break;
        case 0x2E:
            fmt::print("LD L,${:#02x}\n", cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.HL.setLowByte(cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.PC.add(2);
            break;
        case 0x18:
        {
            uint16_t newAddr = cpu_state.PC.getValue() + 2 + static_cast<int8_t>(cpu_state.memory[cpu_state.PC.getValue() + 1]);
            fmt::print("JR Addr_{:02x}\n", newAddr);
            cpu_state.PC.setValue(newAddr);
            break;
        }
        case 0x67:
            fmt::print("LD H,A\n");
            cpu_state.HL.setHighByte(cpu_state.AF.getHighByte());
            cpu_state.PC.add(1);
            break;
        case 0x57:
            fmt::print("LD D,A\n");
            cpu_state.DE.setHighByte(cpu_state.AF.getHighByte());
            cpu_state.PC.add(1);
            break;
        case 0x04:
            fmt::print("INC B\n");
            cpu_state.BC.incrementHighByte();

            if (cpu_state.BC.getHighByte() == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.clearBit(FLAG_SUBTRACT_BIT);
            //todo : half carry

            cpu_state.PC.add(1);
            break;
        case 0x1E:
            fmt::print("LD E,${:#02x}\n", cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.DE.setLowByte(cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.PC.add(2);
            break;
        case 0xF0:
            fmt::print("LD A,($FF00+${:#02x})\n", cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.AF.setHighByte(cpu_state.memory[0xFF00 + cpu_state.memory[cpu_state.PC.getValue() + 1]]);
            cpu_state.PC.add(2);
            break;
        case 0x1D:
            fmt::print("DEC E\n");
            cpu_state.DE.decrementLowByte();

            if (cpu_state.DE.getLowByte() == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.setBit(FLAG_SUBTRACT_BIT);
            // todo: half carry

            cpu_state.PC.add(1);
            break;
        case 0x24:
            fmt::print("INC H\n");
            cpu_state.HL.incrementHighByte();

            if (cpu_state.HL.getHighByte() == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.clearBit(FLAG_SUBTRACT_BIT);
            //todo: half carry

            cpu_state.PC.add(1);
            break;
        case 0x7C:
            fmt::print("LD A,H\n");
            cpu_state.AF.setHighByte(cpu_state.HL.getHighByte());
            cpu_state.PC.add(1);
            break;
        case 0x90:
        {
            fmt::print("SUB B\n");
            int8_t difference = cpu_state.AF.getHighByte() - cpu_state.BC.getHighByte();
            if (difference == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.setBit(FLAG_SUBTRACT_BIT);
            //TODO: half carry
            if (difference < 0)
            {
                cpu_state.AF.setBit(FLAG_CARRY_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_CARRY_BIT);
            }

            cpu_state.AF.subtractHighByte(cpu_state.BC.getHighByte());
            cpu_state.PC.add(1);
        }
            break;
        case 0x15:
            fmt::print("DEC D\n");
            cpu_state.DE.decrementHighByte();

            if (cpu_state.DE.getHighByte() == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.setBit(FLAG_SUBTRACT_BIT);
            //todo: half carry

            cpu_state.PC.add(1);
            break;
        case 0x16:
            fmt::print("LD D,${:#02x}\n", cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.DE.setHighByte(cpu_state.memory[cpu_state.PC.getValue() + 1]);
            cpu_state.PC.add(2);
            break;
        case 0x4F:
            fmt::print("LD C,A\n");
            cpu_state.BC.setLowByte(cpu_state.AF.getHighByte());
            cpu_state.PC.add(1);
            break;
        case 0xC5:
            fmt::print("PUSH BC {:#04x}\n", cpu_state.BC.getValue());

            stack[cpu_state.SP.getValue()] = cpu_state.BC.getHighByte();
            cpu_state.SP.subtract(1);
            stack[cpu_state.SP.getValue()] = cpu_state.BC.getLowByte();
            cpu_state.SP.subtract(1);

            cpu_state.PC.add(1);
            break;
        case 0x17:
        {
            bool hasCarryBitSet = cpu_state.AF.checkBit(FLAG_CARRY_BIT);

            if (cpu_state.AF.checkHighBit(7))
            {
                cpu_state.AF.setBit(FLAG_CARRY_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_CARRY_BIT);
            }

            cpu_state.AF.rotateHighByteLeft();
            if (hasCarryBitSet) cpu_state.AF.incrementHighByte(); // wtf

            cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            cpu_state.AF.clearBit(FLAG_SUBTRACT_BIT);
            cpu_state.AF.clearBit(FLAG_HALF_CARRY_BIT);
            fmt::print("RLA\n");

            cpu_state.PC.add(1);
        }
            break;
        case 0xC1:
            cpu_state.SP.add(1);
            cpu_state.BC.setLowByte(stack[cpu_state.SP.getValue()]);
            cpu_state.SP.add(1);
            cpu_state.BC.setHighByte(stack[cpu_state.SP.getValue()]);
            cpu_state.PC.add(1);
            fmt::print("POP BC {:#04x}\n", cpu_state.BC.getValue());
            break;
        case 0xC9:
            cpu_state.SP.add(1);
            cpu_state.PC.setLowByte(stack[cpu_state.SP.getValue()]);
            cpu_state.SP.add(1);
            cpu_state.PC.setHighByte(stack[cpu_state.SP.getValue()]);
            fmt::print("RET {:#04x}\n", cpu_state.PC.getValue());
            break;
        case 0xBE:
        {
            fmt::print("CP (HL)\n");
            int8_t difference = cpu_state.AF.getHighByte() - cpu_state.memory[cpu_state.HL.getValue()];
            if (difference == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.setBit(FLAG_SUBTRACT_BIT);
            //TODO: half carry
            if (difference < 0)
            {
                cpu_state.AF.setBit(FLAG_CARRY_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_CARRY_BIT);
            }

            cpu_state.PC.add(1);
            break;
        }
        case 0x7D:
            fmt::print("LD A,L\n");
            cpu_state.AF.setHighByte(cpu_state.HL.getLowByte());
            cpu_state.PC.add(1);
            break;
        case 0x78:
            fmt::print("LD A,B\n");
            cpu_state.AF.setHighByte(cpu_state.BC.getHighByte());
            cpu_state.PC.add(1);
            break;
        case 0x86:
            fmt::print("ADD A,(HL)\n");

            if (cpu_state.AF.getValue() + cpu_state.memory[cpu_state.HL.getValue()] > 0b10000000)
            {
                cpu_state.AF.setBit(FLAG_CARRY_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_CARRY_BIT);
            }

            cpu_state.AF.addHighByte(cpu_state.memory[cpu_state.HL.getValue()]);
            if (cpu_state.AF.getHighByte() == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.clearBit(FLAG_SUBTRACT_BIT);
            //todo: half carry
            
            cpu_state.PC.add(1);
            break;
        case 0x00:
            fmt::print("NOP\n");
            cpu_state.PC.add(1);
            break;
        case 0x3c:
            fmt::print("INC A\n");
            cpu_state.AF.incrementHighByte();

            if (cpu_state.AF.getHighByte() == 0)
            {
                cpu_state.AF.setBit(FLAG_ZERO_BIT);
            }
            else
            {
                cpu_state.AF.clearBit(FLAG_ZERO_BIT);
            }
            cpu_state.AF.clearBit(FLAG_SUBTRACT_BIT);
            //todo :half carry

            cpu_state.PC.add(1);
            break;
        default:
            fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "UNKNOWN: {:#02x}\n", instruction);
            fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "EMULATION ERROR, EXITING\n");
            return -1;
        }

        fmt::print("================================\n");

    }
    fmt::print("PARSING ROM COMPLETE\n");

    return 0;
}