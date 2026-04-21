![Snailmon logo](.readme-res/sm-ico2-us.png) 
## Snailmon - Pokémon Mini emulator for the GBA

This is a very primitive Pokémon Mini emulator for the GBA that runs very slow (hence the name).
Although the CPU runs at about 40% speed, some simple games are playable at nearly full speed, see "COMPATIBILITY.md".
> ⚠️ **Photosensitivity warning**
> 
> Many Pokémon Mini games use a technique that strobes some parts of the image to simulate more shades of gray on the screen. Snailmon is not fast enough to do this at 60Hz so the strobing is going to be very noticeable. **If you are sensible to flashing images please do not try this!**

### Usage
1. Install the DevkitARM toolchain from [DevkitPro](https://devkitpro.org/wiki/Getting_Started)
2. Create a folder named "bin" where the Makefile is located
3. In the "bin" folder, put the PM bios file as "bios.bin" and your ROM as "rom.bin". The project won't compile without both of these. You can use [FreeBIOS](https://github.com/pokemon-mini/pokemini/blob/master/freebios/freebios.min) instead of the official one
5. Run the `make` command from where the Makefile is located
  
**Everytime you change the ROM** you need to recompile the project again. To do that make sure your run `make clean` before running `make` again

### Controls
`PM input --- GBA input`

		`A --> A`
		
		`B --> B`
		
		`C --> R trigger`
		
	 `DPAD --> DPAD`
	 
	`Power --> START`
	
	`Shock --> L trigger`
	
Press SELECT to access the ingame menu

### Saving your progress
Please note that some commercial games only save your progress to EEPROM when you put the system to sleep. To do that with Snailmon, remember to press START and wait until your GBA screen turns off. Now your progress is saved and you can press START again to resume.

### Per game optimization
Some games will require changes in the settings at compile time in order to make them playable.
To do this, you need to edit the file "src/oflags.h", here are the options currently implemented:
- GFX_SPR_FULLSET
	
	Set this to 1 if the sprites show up with the wrong graphics, set this to 0 if the game runs extremely slow at times
	(example: Cortex needs this set to 0, while most commercial games need this set to 1)
- INSTR_FETCH_SPEEDUP
	
	It is reccomended to leave this set to 1 for greater performance, unless your game has to execute some code that is in RAM, then you can set it to 0
- MINXCPU_DEBUG
	
	DO NOT ENABLE THIS unless you are a developer looking to debug the emulator, this will slow down the emulation a lot! (reccomended: 0)
