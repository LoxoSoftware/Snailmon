## Snailmon - Pokèmon Mini emulator for the GBA
This is a very primitive Pokèmon Mini emulator for the GBA that runs very slow (hence the name)

### Usage
1. Install the DevkitARM toolchain from DevkitPro
2. Create a folder named "bin" where the Makefile is located
3. In the "bin" folder, put the PM bios file as "bios.bin" and your ROM as "rom.bin". The project won't compile without both of these. You can use [FreeBIOS](https://github.com/pokemon-mini/pokemini/blob/master/freebios/freebios.min) instead of the official one
5. Run the `make` command from where the Makefile is located
  
**Everytime you change the ROM** you need to recompile the project again. To do that make sure your run `make clean` before running `make` again
