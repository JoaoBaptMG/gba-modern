# Running on windows is not supported
ifdef OS
	$(error "Making this project on Windows is not yet supported. Consider using WSL.")
endif

# Utility function
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# Compilation flags
ARCH := -mthumb -mthumb-interwork
CFLAGS := -g -Wall -O3 -ffunction-sections -fdata-sections -mcpu=arm7tdmi -mtune=arm7tdmi -flto $(ARCH)
CPPFLAGS := $(CFLAGS) -std=c++17 -fno-rtti -fno-exceptions
ASFLAGS := -g $(ARCH)
LDFLAGS	= -g $(ARCH) -Wl,--gc-sections -Wl,-Map,gba.map

# Libraries
TONC := tonclib/code/tonclib
LIBRARIES := -nodefaultlibs -ltonc
LIBDIRS := $(TONC)
INCDIRS := external/gcem/include

# Source files
C_FILES := $(call rwildcard, source/, *.c)
CPP_FILES := $(call rwildcard, source/, *.cpp)
S_FILES := $(call rwildcard, source/, *.s)

# Source objects
SRC_OFILES = $(addprefix build/, $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(S_FILES:.s=.o))

# Resources
SPR_FILES := $(call rwildcard, data/sprites/, *.png)
BKG_FILES := $(call rwildcard, data/backgrounds/, *.png)
FNT_FILES := $(call rwildcard, data/fonts/, *.ttf)
AUS_FILES := data/audio.json
SND_FILES := $(call rwildcard, data/sounds/, *.wav)

# Resource objects
RSRC_OFILES := $(addprefix build/, $(SPR_FILES:.png=.o) $(BKG_FILES:.png=.o) $(FNT_FILES:.ttf=.o)\
	$(SND_FILES:.wav=.o)) 

# Resource headers
RSRC_HFILES := $(addprefix build/, $(SPR_FILES:.png=.hpp) $(BKG_FILES:.png=.hpp) $(FNT_FILES:.ttf=.hpp)\
	data/audio-settings.hpp $(SND_FILES:.wav=.hpp))

# Helper variables
OFILES := $(RSRC_OFILES) $(SRC_OFILES)
DFILES := $(OFILES:.o=.d)
LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
INCLUDE	:= $(foreach dir,$(LIBDIRS),-isystem $(dir)/include) $(foreach dir,$(INCDIRS), -isystem $(dir)) -iquote build -iquote source

# Links to Tonc
TONC_URL := http://www.coranac.com/files/tonc-code.zip

# Link to the GCC - use architecture to pull the right one
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		GCC_URL := https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		GCC_URL := https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-aarch64-linux.tar.bz2
	endif
endif
ifeq ($(UNAME_S),Darwin)
	GCC_URL := https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-mac.tar.bz2
endif
GCC_DEST := gcc-arm-none-eabi-9-2019-q4-major

# Link to libsamplerate
LSRC_VERSION = 0.1.9
LSRC_URL = http://www.mega-nerd.com/SRC/libsamplerate-$(LSRC_VERSION).tar.gz

# Path to the tools used
ARMCC := $(GCC_DEST)/bin/arm-none-eabi-gcc
ARMCPP := $(GCC_DEST)/bin/arm-none-eabi-g++
ARMOC := $(GCC_DEST)/bin/arm-none-eabi-objcopy
ARMOD := $(GCC_DEST)/bin/arm-none-eabi-objdump

# Get the right linker
ifeq ($(strip $(CPPFILES)),)
	ARMLD := $(ARMCC)
else
	ARMLD := $(ARMCPP)
endif

# Keep all intermediary files (maps, assembly and such) around
.SECONDARY:

# Enable second expansion rule
.SECONDEXPANSION:

# Now, the actual rules
.PHONY: all clean clean-everything clean-downloads download-deps build-tools

all: bin/game.gba

# Now, make the game
bin/game.gba: download-deps build-tools tools/tools bin/game.elf
	@mkdir -p bin
	@echo "Sanitizing final file"
	@$(ARMOC) -O binary $(@:.gba=.elf) $@
	@tools/tools rom-sanitize $@ gba.json $@

bin/game.elf: build/data/audio-settings.hpp $(OFILES)
	@mkdir -p bin
	@echo "Linking"
	@$(ARMLD) $(LDFLAGS) -specs=gba.specs $(filter-out %crt0.o, $(OFILES)) $(LIBPATHS) $(LIBRARIES) -o $@
	@$(ARMOD) -dCS $@ > $@.dump

-include $(DFILES)

build/data/%.o: build/data/%.s
	@mkdir -p $(@D)
	$(ARMCC) -MMD -MP -MF $(@:.o=.d) -x assembler-with-cpp $(ASFLAGS) -c $< -o $@

build/data/sprites/%.s build/data/sprites/%.hpp: data/sprites/%.png tools/tools
	@mkdir -p $(@D)
	tools/tools sprite-export $(filter %.png,$^) $(basename $@).s $(basename $@).hpp

build/data/backgrounds/%.s build/data/backgrounds/%.hpp: data/backgrounds/%.png tools/tools
	@mkdir -p $(@D)
	tools/tools background-export $(filter %.png,$^) $(basename $@).s $(basename $@).hpp

build/data/fonts/%.s build/data/fonts/%.hpp: data/fonts/%.ttf data/fonts/%.ttf.json tools/tools
	@mkdir -p $(@D)
	tools/tools font-export $(filter %.ttf,$^) $(basename $@).s $(basename $@).hpp

build/data/audio-settings.hpp build/data/audio-settings.json: data/audio.json tools/tools 
	@mkdir -p $(@D)
	tools/tools audio-export-settings $(filter %.json,$^) $(basename $@).hpp $(basename $@).json

build/data/sounds/%.s build/data/sounds/%.hpp: data/sounds/%.wav build/data/audio-settings.json tools/tools
	@mkdir -p $(@D)
	tools/tools sound-export $(filter %.wav,$^) $(basename $@).s $(basename $@).hpp build/data/audio-settings.json

# Source files
build/%.iwram.o: %.iwram.cpp
	@mkdir -p $(@D)
	$(ARMCPP) -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@

build/%.iwram.o: %.iwram.c
	@mkdir -p $(@D)
	$(ARMCC) -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@

build/%.o: %.cpp
	@mkdir -p $(@D)
	$(ARMCPP) -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

build/%.o: %.c
	@mkdir -p $(@D)
	$(ARMCC) -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -c $< -o $@

build/%.o: %.s
	@mkdir -p $(@D)
	$(ARMCC) -MMD -MP -MF $(@:.o=.d) -x assembler-with-cpp $(ASFLAGS) -c $< -o $@

download-deps: $(GCC_DEST) tonc libsamplerate

# Pull tonc
tonc: tonc.zip
	unzip -qq -n tonc.zip 'code/tonclib/*' -d tonclib

tonc.zip:
	wget $(TONC_URL) -O tonc.zip

# Pull gcc
$(GCC_DEST): gcc.tar.bz2
ifeq (,$(GCC_URL))
	$(error "This platform does not have a prebuilt arm-none-eabi; try to compile it from source and place it at $(GCC_DEST)")
endif
	tar xjf gcc.tar.bz2

gcc.tar.bz2:
	wget $(GCC_URL) -O gcc.tar.bz2

# Pull libsamplerate
libsamplerate: libsamplerate.tar.gz
	mkdir libsamplerate
	tar xzf libsamplerate.tar.gz -C libsamplerate --strip-components 1

libsamplerate.tar.gz:
	wget $(LSRC_URL) -O libsamplerate.tar.gz

# Tools
tools/tools: build-tools

build-tools: libsamplerate
	$(MAKE) -C tools

clean:
	rm -rf bin build

clean-everything:
	$(MAKE) -C tools clean
	rm -rf bin build

clean-downloads:
	$(MAKE) -C tools clean
	rm -rf bin build tonc* gcc* libsamplerate
