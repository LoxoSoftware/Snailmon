![alt text](.readme-res/sm-ico2-us.png) 
## Snailmon - Pokémon Mini emulator for the GBA

This is a very primitive Pokémon Mini emulator for the GBA that runs very slow (hence the name).
Although the CPU runs at about 27% speed, some simple games are playable at nearly full speed, see "COMPATIBILITY.md".
> ⚠️ **Photosensitivity warning**
> 
> Many Pokémon Mini games use a technique that strobes some parts of the image to simulate more shades of gray on the screen. Snailmon is not fast enough to do this at 60Hz so the strobing is going to be very noticeable. **If you are sensible to flashing colors please do not try this!**

### Usage
1. Install the DevkitARM toolchain from [DevkitPro](https://devkitpro.org/wiki/Getting_Started)
2. Create a folder named "bin" where the Makefile is located
3. In the "bin" folder, put the PM bios file as "bios.bin" and your ROM as "rom.bin". The project won't compile without both of these. You can use [FreeBIOS](https://github.com/pokemon-mini/pokemini/blob/master/freebios/freebios.min) instead of the official one
5. Run the `make` command from where the Makefile is located
  
**Everytime you change the ROM** you need to recompile the project again. To do that make sure your run `make clean` before running `make` again

