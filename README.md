# Xbox 360 STM32 NAND Flasher

[G33KatWork's AVR Flasher](https://github.com/G33KatWork/XBox-360-AVR-flasher) ported to STM32 Blue Pill (STM32F103).  
Supports 16MB NANDs and it's pretty fast (~70s for a full R/W)
Added POST logger support (needs a 1v2->3v3 inverting shifter, you can build one with 8x BC337)

## Pinout
### SPI
- PA2: XXX
- PA3: XEJ
- PA4: SS
- PA5: SCK
- PA6: MISO
- PA7: MOSI

### POST
- PB9: POST BIT 0
- PB7: POST BIT 1
- PB5: POST BIT 2
- PB13: POST BIT 3
- PB8: POST BIT 4
- PB6: POST BIT 5
- PB4: POST BIT 6
- PB14: POST BIT 7

# Mode
You can switch between NAND-X and JR-P modes by moving the jumper on BOOT1 header
- 0: NAND-X/MTX Mode
- 1: JR-Programmer V2 mode
