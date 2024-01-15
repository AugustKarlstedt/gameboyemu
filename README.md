![Nintendo Logo)
](https://augustkarlstedt.github.io/wp-content/uploads/2020/04/nintendo.gif)
https://augustkarlstedt.github.io/blog/2020/04/17/emulators-are-cool/

Instruction set:
- https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html

First ROM to implement:
- https://gbdev.gg8.se/wiki/articles/Gameboy_Bootstrap_ROM#Contents_of_the_ROM

Test roms:
- https://github.com/retrio/gb-test-roms

GameBoy CPU manual:
- http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf

Good info on Z80: 
- http://z80-heaven.wikidot.com/flags-and-bit-level-instructions

Diagram:
- https://www.copetti.org/writings/consoles/game-boy/

==Scratchpad==

```
0x80 = 1000 0000
0xF3 = 1111 0011
0x77 = 0111 0111

LD HL,$ff26		; $000c  Setup Audio       HL = 0xFF26            					sound on/off register. 
LD C,$11		; $000f					   C = 0x11
LD A,$80		; $0011                    A = 0x80
LD (HL-),A		; $0013                    memory[0xFF26] = 0x80; HL = 0xFF25;      set all sound on
LD ($FF00+C),A	; $0014                    memory[0xFF00+0x11] = 0x80               sound mode 1 register: length/wave pattern duty
INC C			; $0015                    C = 0x12
LD A,$f3		; $0016                    A = 0xF3
LD ($FF00+C),A	; $0018                    memory[0xFF00+0x12] = 0xF3               sound mode 1 register: envelope
LD (HL-),A		; $0019                    memory[0xFF25] = 0xF3; HL = 0xFF24       set sound output terminal: output [4,3,2,1] to SO2 and output [2,1] to SO1
LD A,$77		; $001a                    A = 0x77
LD (HL),A		; $001c                    memory[0xFF24] = 0x77                    set SO2 off, but max volume. set SO1 off, but max volume


0x83 = 1000 0011
0x87 = 1000 0111

play sound #1...
LD C,$13		; $0070					  C = 0x13
LD E,$83		; $0074					  E = 0x83
LD A,E			; $0080  play sound		  A = 0x83
LD ($FF00+C),A	; $0081            	      memory[0xFF00+0x13] = 0x83			    sound mode 1 register: frequency lo		    
INC C			; $0082					  C = 0x14
LD A,$87		; $0083					  A = 0x87
LD ($FF00+C),A	; $0085					  memory[0xFF00+0x14] = 0x87				sound mode 1 register: frequency hi + enable consecutive

```