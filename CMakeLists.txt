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
	$<TARGET_OBJECTS:GAME_OBJECTS>
	$<TARGET_OBJECTS:RESOURCES>)
#DFILES := $(OFILES:.o=.d)
#LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
#INCLUDE	:= $(foreach dir,$(LIBDIRS),-isystem $(dir)/include) $(foreach dir,$(INCDIRS), -isystem $(dir)) -iquote build -iquote source

find_package(LibSampleRate REQUIRED)

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
