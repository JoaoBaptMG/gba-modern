# Projects Settings
cmake_minimum_required (VERSION 3.1)

project (pk3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(tools)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
function(download_file url filename)

if(NOT EXISTS ${filename})
	file(DOWNLOAD ${url} ${filename}
		TIMEOUT 60
		TLS_VERIFY ON)
endif()

endfunction(download_file)

# Compilation flags
#ARCH := -mthumb -mthumb-interwork
#CFLAGS := -g -Wall -O3 -ffunction-sections -fdata-sections -mcpu=arm7tdmi -mtune=arm7tdmi -flto $(ARCH)
#CPPFLAGS := $(CFLAGS) -std=c++17 -fno-rtti -fno-exceptions
#ASFLAGS := -g $(ARCH)
#LDFLAGS	= -g $(ARCH) -Wl,--gc-sections -Wl,-Map,gba.map

set(GCC_PREFIX "10-2020q2")
set(GCC_NAME "gcc-arm-none-eabi-10-2020-q2-preview")
set(GCC_URL 
	"https://developer.arm.com/-/media/Files/downloads/gnu-rm/${GCC_PREFIX}/${GCC_NAME}-x86_64-linux.tar.bz2")

# Set Tonc
set(TONC_URL
	"http://www.coranac.com/files/tonc-code.zip")

# Set libsamplerate
set(LSRC_VERSION "0.1.9" CACHE STRING "LSRC_VERSION")
set(LSRC_URL "http://www.mega-nerd.com/SRC/libsamplerate-${LSRC_VERSION}.tar.gz" CACHE STRING "LSRC_URL")


# Libraries
#TONC := tonclib/code/tonclib
#LIBRARIES := -nodefaultlibs -ltonc
#LIBDIRS := $(TONC)
#INCDIRS := external/gcem/include

# Source files
# C Files
set(C_FILES
	source/math/gcd.c
	source/util/libc-replacement.c
	source/util/memmove.iwram.c
	source/util/random.iwram.c)
# CPP Files
set(CPP_FILES
	source/main.cpp
	source/audio/audio.cpp
	source/gameplay/Background.cpp
	source/gameplay/collisionCircleBitmaskGenerator.cpp
	source/gameplay/Enemy.cpp
	source/gameplay/EnemyProjectiles.cpp
	source/gameplay/Explosions.cpp
	source/gameplay/GameScene.cpp
	source/gameplay/Hud.cpp
	source/gameplay/Level.cpp
	source/gameplay/Player.cpp
	source/gameplay/PlayerProjectiles.cpp
	source/gameplay/Projectile.iwram.cpp
	source/gameplay/levels/level1/bigLurker.cpp
	source/gameplay/levels/level1/level.cpp
	source/gameplay/levels/level1/wavingEnemy.cpp
	source/gameplay/levels/level1/wobblingEnemies.cpp
	source/graphics/BuddyObjectAllocator.cpp
	source/graphics/FrameAnimator.cpp
	source/graphics/graphics.cpp
	source/graphics/HblankDma.cpp
	source/graphics/OamManager.cpp
	source/graphics/OamManager.iwram.cpp
	source/graphics/StreamAnimator.cpp
	source/math/trapdiv0.cpp
	source/memory/allocator.iwram.cpp
	source/scenes/BitmaskTestScene.cpp
	source/scenes/ContextTestScene.cpp
	source/scenes/Mode3TextTestScene.cpp
	source/scenes/TileTextTestScene.cpp
	source/util/dynamic.cpp
	source/util/profile.cpp
	source/util/virtual.cpp)
# S Files
set(S_FILES
	source/crt0.s
	source/audio/audioMix.s
	source/gameplay/collision.s
	source/gameplay/collisionBitmask.s
	source/gameplay/Projectile.s
	source/math/clz.s
	source/math/lmul.s
	source/math/sdiv32.s
	source/math/sqrt32.s
	source/math/sqrt64.s
	source/math/udiv32.s
	source/text/mode3PutGlyph.s
	source/text/tile4bppPutGlyph.s
	source/util/context.s
	source/util/memops.s
	source/util/uintDigits.s)

# Source objects
#SRC_OFILES = $(addprefix build/, $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(S_FILES:.s=.o))

# Resources
# Sprite Files
set(SPR_FILES
	data/sprites/big-lurker.png
	data/sprites/bitmask-test.png
	data/sprites/enemy-projectiles.png
	data/sprites/hud.png
	data/sprites/player.png
	data/sprites/player-projectiles.png
	data/sprites/small-explosion.png
	data/sprites/waving-enemy.png
	data/sprites/wobbling-enemy.png)
# Background Files
set(BKG_FILES
	data/backgrounds/level.png)
# Font Files
set(FNT_FILES
	data/fonts/monogram_extended.ttf)
# Audio Settings
set(AUS_FILES
	data/audio.json)
# Sound Files
set(SND_FILES
	data/sounds/player-dmg.wav
	data/sounds/player-heal.wav
	data/sounds/player-shot.wav)

# Resource objects
#RSRC_OFILES := $(addprefix build/, $(SPR_FILES:.png=.o) $(BKG_FILES:.png=.o) $(FNT_FILES:.ttf=.o)\
#	$(SND_FILES:.wav=.o)) 

# Resource headers
#RSRC_HFILES := $(addprefix build/, $(SPR_FILES:.png=.hpp) $(BKG_FILES:.png=.hpp) $(FNT_FILES:.ttf=.hpp)\
#	data/audio-settings.hpp $(SND_FILES:.wav=.hpp))

# Helper variables
#OFILES := $(RSRC_OFILES) $(SRC_OFILES)
#DFILES := $(OFILES:.o=.d)
#LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
#INCLUDE	:= $(foreach dir,$(LIBDIRS),-isystem $(dir)/include) $(foreach dir,$(INCDIRS), -isystem $(dir)) -iquote build -iquote source

# Path to the tools used
set(ARMCC gcc/bin/arm-none-eabi-gcc CACHE PATH "ARMCC")
#ARMCC := gcc/bin/arm-none-eabi-gcc
set(ARMCPP gcc/bin/arm-none-eabi-g++ CACHE PATH "ARMCPP")
#ARMCPP := gcc/bin/arm-none-eabi-g++
set(ARMOC gcc/bin/arm-none-eabi-objcopy CACHE PATH "ARMOC")
#ARMOC := gcc/bin/arm-none-eabi-objcopy
set(ARMOD gcc/bin/arm-none-eabi-objdump CACHE PATH "ARMOD")
#ARMOD := gcc/bin/arm-none-eabi-objdump

# Get the right linker
#ifeq ($(strip $(CPPFILES)),)
#	ARMLD := $(ARMCC)
#else
#	ARMLD := $(ARMCPP)
#endif

# Keep all intermediary files (maps, assembly and such) around
#.SECONDARY:

# Enable second expansion rule
#.SECONDEXPANSION:

# Now, the actual rules
#.PHONY: all clean clean-everything clean-downloads download-deps build-tools

#all: bin/game.gba

# Now, make the game
#bin/game.gba: download-deps build-tools tools/tools bin/game.elf
#	@mkdir -p bin
#	@echo "Sanitizing final file"
#	@$(ARMOC) -O binary $(@:.gba=.elf) $@
#	@tools/tools rom-sanitize $@ gba.json $@

#bin/game.elf: build/data/audio-settings.hpp $(OFILES)
#	@mkdir -p bin
#	@echo "Linking"
#	@$(ARMLD) $(LDFLAGS) -specs=gba.specs $(filter-out %crt0.o, $(OFILES)) $(LIBPATHS) $(LIBRARIES) -o $@
#	@$(ARMOD) -dCS $@ > $@.dump

#-include $(DFILES)

#build/data/%.o: build/data/%.s
#	@mkdir -p $(@D)
#	$(ARMCC) -MMD -MP -MF $(@:.o=.d) -x assembler-with-cpp $(ASFLAGS) -c $< -o $@

#build/data/sprites/%.s build/data/sprites/%.hpp: data/sprites/%.png tools/tools
#	@mkdir -p $(@D)
#	tools/tools sprite-export $(filter %.png,$^) $(basename $@).s $(basename $@).hpp

#build/data/backgrounds/%.s build/data/backgrounds/%.hpp: data/backgrounds/%.png tools/tools
#	@mkdir -p $(@D)
#	tools/tools background-export $(filter %.png,$^) $(basename $@).s $(basename $@).hpp

#build/data/fonts/%.s build/data/fonts/%.hpp: data/fonts/%.ttf data/fonts/%.ttf.json tools/tools
#	@mkdir -p $(@D)
#	tools/tools font-export $(filter %.ttf,$^) $(basename $@).s $(basename $@).hpp

#build/data/audio-settings.hpp build/data/audio-settings.json: data/audio.json tools/tools 
#	@mkdir -p $(@D)
#	tools/tools audio-export-settings $(filter %.json,$^) $(basename $@).hpp $(basename $@).json

#build/data/sounds/%.s build/data/sounds/%.hpp: data/sounds/%.wav build/data/audio-settings.json tools/tools
#	@mkdir -p $(@D)
#	tools/tools sound-export $(filter %.wav,$^) $(basename $@).s $(basename $@).hpp build/data/audio-settings.json

# Source files
#build/%.iwram.o: %.iwram.cpp
#	@mkdir -p $(@D)
#	$(ARMCPP) -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@

#build/%.iwram.o: %.iwram.c
#	@mkdir -p $(@D)
#	$(ARMCC) -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@

#build/%.o: %.cpp
#	@mkdir -p $(@D)
#	$(ARMCPP) -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

#build/%.o: %.c
#	@mkdir -p $(@D)
#	$(ARMCC) -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -c $< -o $@

#build/%.o: %.s
#	@mkdir -p $(@D)
#	$(ARMCC) -MMD -MP -MF $(@:.o=.d) -x assembler-with-cpp $(ASFLAGS) -c $< -o $@

#download-deps: gcc tonc libsamplerate

# Pull tonc
#tonc: tonc.zip
#	unzip -qq -n tonc.zip 'code/tonclib/*' -d tonclib

#tonc.zip:
#	wget $(TONC_URL) -O tonc.zip

# Pull gcc
#gcc: gcc.tar.bz2
#ifeq (,$(GCC_PLATFORM))
#	$(error "This platform does not have a prebuilt arm-none-eabi; try to compile it from source and place it at $(GCC_NAME)")
#endif
#	mkdir gcc && tar xjf gcc.tar.bz2 -C gcc --strip-components 1

#gcc.tar.bz2:
#	wget $(GCC_URL) -O gcc.tar.bz2

# Pull libsamplerate
#libsamplerate: libsamplerate.tar.gz
#	mkdir libsamplerate && tar xzf libsamplerate.tar.gz -C libsamplerate --strip-components 1

#libsamplerate.tar.gz:
#	wget $(LSRC_URL) -O libsamplerate.tar.gz

# Tools
#tools/tools: build-tools

#build-tools: libsamplerate
#	$(MAKE) -C tools

#clean:
#	rm -rf bin build

#clean-everything:
#	$(MAKE) -C tools clean
#	rm -rf bin build

#clean-downloads:
#	$(MAKE) -C tools clean
#	rm -rf bin build tonc* gcc* libsamplerate*
