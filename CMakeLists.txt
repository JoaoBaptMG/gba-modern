# Projects Settings
cmake_minimum_required (VERSION 3.16)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$CMAKE_SOURCE_DIR}")
project (gbaModern)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compilation flags
set(ARCH "-mthumb -mthumb-interwork")
set(CFLAGS "-g -Wall -O3 -ffunction-sections -fdata-sections -mcpu=arm7tdmi -mtune=arm7tdmi -flto ${ARCH}")
set(CPPFLAGS "${CFLAGS} -std=c++17 -fno-rtti -fno-exceptions")
set(ASFLAGS "-g ${ARCH}")
set(LDFLAGS	"-g ${ARCH} -Wl,--gc-sections -Wl,-Map,gba.map")

set(GCC_PREFIX "10-2020q2")
set(GCC_NAME "gcc-arm-none-eabi-10-2020-q2-preview")
set(GCC_URL 
	"https://developer.arm.com/-/media/Files/downloads/gnu-rm/${GCC_PREFIX}/${GCC_NAME}-x86_64-linux.tar.bz2")

# Set Tonc
set(TONC_URL
	"http://www.coranac.com/files/tonc-code.zip")

# Libraries
set(TONC_PATH "${CMAKE_SOURCE_DIR}/external/tonc/tonclib")
set(LIBRARIES_ "-nodefaultlibs -ltonc")
set(LIBDIRS ${TONC_PATH})
set(INCDIRS "${CMAKE_SOURCE_DIR}/external/gcem/include")


add_subdirectory(tools)
add_subdirectory(data)
add_subdirectory(source)


# Helper variables
add_library(OBJECT_FILES OBJECT
	${SPR_S}
	${SPR_HPP}
	${BKG_S}
	${BKG_HPP}
	${FNT_S}
	${FNT_HPP}
	${AUS_HPP}
	${AUS_JSON}
	${SND_S}
	${SND_HPP}
	${GAME_C_FILES}
	${GAME_CPP_FILES}
	${GAME_S_FILES})
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

#wget $(GCC_URL) -O gcc.tar.bz2
#ifeq (,$(GCC_PLATFORM))
#	$(error "This platform does not have a prebuilt arm-none-eabi; try to compile it from source and place it at $(GCC_NAME)")
#endif
#	mkdir gcc && tar xjf gcc.tar.bz2 -C gcc --strip-components 1

#wget $(TONC_URL) -O tonc.zip
#unzip -qq -n tonc.zip 'code/tonclib/*' -d tonclib

#wget $(LSRC_URL) -O libsamplerate.tar.gz
#mkdir libsamplerate && tar xzf libsamplerate.tar.gz -C libsamplerate --strip-components 1

#$(MAKE) -C tools
#@mkdir -p bin
#@echo "Linking"
#bin/game.elf: build/data/audio-settings.hpp $(OFILES)
#	@$(ARMLD) $(LDFLAGS) -specs=gba.specs $(filter-out %crt0.o, $(OFILES)) $(LIBPATHS) $(LIBRARIES) -o $@
#	@$(ARMOD) -dCS $@ > $@.dump
#bin/game.gba:
#	@mkdir -p bin
#	@echo "Sanitizing final file"
#	@$(ARMOC) -O binary $(@:.gba=.elf) $@
#	@tools/tools rom-sanitize $@ gba.json $@



#-include $(DFILES)

#build/data/%.o: build/data/%.s
#	@mkdir -p $(@D)
#	$(ARMCC) -MMD -MP -MF $(@:.o=.d) -x assembler-with-cpp $(ASFLAGS) -c $< -o $@

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
