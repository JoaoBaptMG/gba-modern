gba-modern-template
==========
The objective of this project is to aim to create a Game Boy Advance game using C++17, while employing generators to the assets of the game.

Building
========
**Warning:** currently the project does not work in a Windows environment! Use WSL to build it on Windows.

**Warning:** this repository uses submodules, be sure to initialize them properly by cloning this repo using the `--recursive` or then using the command `git submodule update --init` after cloning it!

This project consists in a toolkit written in C++17 to generate assembly files for the resources (tilesets, maps and sprites as of writing), and it also includes the source files of the game, written in C++17. The included *Makefile* makes sure all the files are properly built. It requires a `g++` which supports C++17 (possibly every Linux distribution has one). **It also requires the `freetype` library!** Once all of those are done, you can properly `make` the project, and the ROM will be available in the path _bin/game.gba_.

Screenshots and Videos
======================

[Working video](videos/video-0.mp4)

GBA Hardware Specifications
============
**CPU** -- 32-bit ARM7tdmi chip running at 6.78MHz.
The GBA runs on a ARM7tdmi RISC chip at 16.78 MHz (2^24 cycle/second). It is a 32bit cchip that can run on two different instruction sets.
  - ***ARM code*** which is a set of 32 bit instructions.
  - ***THUMB*** which uses 16bit instructions.
***THUMB*** instructions are a subset of the ARM instrution set; since the instructions are shorter, the code can be smaller, but their power is also reduced. It is recommended that normal code be THUMB code in ROM, and for time-critical code to be ARM code and put in IWRAM.

**Memory Sections**
| area | start | end | length | port-size | description |
|---|---|---|---|---|---|
| System ROM | 0000:0000h | 0000:3FFFh | 16kb | 32bit | Bios memory. You can execute it, but not read it (i.o.w, touch, don't look). |
| EWRAM | 0200:0000h | 0203:FFFFh | 256kb | 16bit | External work RAM. Is available for you code and data. If you're using a multiboot cable, this is where the downloaded code goes and execution starts (normally execution starts at ROM). Due to the 16bit nature, you want this section's code to be **THUMB** code. |
| IWRAM | 0300:0000h | 0300:7FFFh | 32kb | 32bit | This is also available for code and data. The 32-bit bus and  the fact that it's embedded in the CPU make this the fastest memory section. The 32-bit bus means that ARM instructions can be loaded at once, so put your ARM code here. |
| IO RAM | 0400:0000h | 0400:03FFh | 1kb | 16bit | Memory-mapped IO registers. These have nothing to do with the CPU registers you use in assembly so the name can be a bit confusing. Don't blame me for that. This section is where you control graphic, sound, buttons and other features. |
| PAL RAM | 0500:0000h | 0500:03FFh | 1kb | 16bit | Memory for two palettes containing 256 entries of 15bit colours each. The first is for backgrounds, the second for sprites. |
| VRAM | 0600:0000h | 0601:7FFFh | 96kb | 16bit | Video RAM. This is where the data used for backgrounds and sprites are stored. The interpretation of this data depends on a number  of things, including video mode and background and sprite settings. |
| OAM | 0700:0000h |  0700:03FFh | 1kb | 32bit | Object Attribute Memory. This is where you control the sprites. |
| PAK ROM | 0800:0000h | var | var | 16bit | Game Pak ROM. This is where the game is located and execution starts, except when you're running from a multiboot cable. The size is variable, but the limit is 32MB. It's a 16bit bus, so **THUMB** code is preferable over **ARM** code here. |
| Cart ROM | 0E00:0000h | var | var | 8bit | This is where saved data is stored. Cart RAM can be in the form of SRAM, Flash ROM or EEPROM. Programmatically they all dothe same thing: store data. The total size is variable, but 64kb is a good indication. |

The various RAM sections (apart from Cart RAM) are zeroed at start-up by BIOS. The areas you will deal with them most are IO, PAL, VRAM and OAM. For simple games and demos it will usually suffice to load your graphics data into PAL and VRAM at the start use IO and OAM to take care of the actual interaction. The layout of these two sections is quite complex and almost impossible to figure out on your own (almost, because emulator builders have obviously done just that). With this in mind, reference sheets like the [GBATek](https://problemkaputt.de/gbatek.htm) and the [CowBite Spec](https://www.cs.rit.edu/~tjh8300/CowBite/CowBiteSpec.htm) are unmissable documents. In theory this is all you need to get you started, but in practice using on or more tutorials with example code (like [Tonc](https://www.coranac.com/tonc/text/)) will save a lot of headaches.

Contributing
============
All contribution is welcome. If you have interesting things to add, please submit a pull request!

Credits
=======
All files in this repository with exception of specific libraries and the fonts located in the _fonts_ foder are Copyright (c) 2019 João Baptista de Paula e Silva and are under the MIT license.

Niels Lohmann's "JSON for Modern C++" library can be found [here](https://github.com/nlohmann/json) and is also under the MIT license.

The LodePNG files belong to Lode Vandevenne and can be found [here](https://lodev.org/lodepng/) also under the zlib license.

The font _monogram.ttf_ belongs to Vinícius Menézio can be found [here](https://datagoblin.itch.io/monogram) under the CC0.

The library GCE-Math used by generating LUTs belongs to Keith O'Hara and can be found [here](https://github.com/kthohr/gcem) under the Apache-2 License.

The _CMakeLists.txt_  and related files are contribution of [Xett](https://github.com/Xett).
