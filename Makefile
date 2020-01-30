#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/gba_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary data
# GRAPHICS is a list of directories containing files to be processed by grit
#
# All directories are specified relative to the project directory where
# the makefile is found
#
#---------------------------------------------------------------------------------
TARGET		:= $(notdir $(CURDIR))
BUILD		:= build
SOURCES		:= source
INCLUDES	:= include
DATA		:= data
MAPS		:= tiled-maps
MUSIC		:=

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb -mthumb-interwork

CFLAGS	:=	-g -Wall -O3 -ffunction-sections -fdata-sections\
		-mcpu=arm7tdmi -mtune=arm7tdmi -flto -fno-omit-frame-pointer $(ARCH)

CFLAGS	+=	$(INCLUDE)

CXXFLAGS	:=	$(CFLAGS) -std=c++2a -fno-rtti -fno-exceptions

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-g $(ARCH) -Wl,--gc-sections -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -nodefaultlibs -ltonc

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(DEVKITPRO)/libtonc $(LIBGBA)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------


ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

# Recursive wildcard
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# Resources
export SPR_FOLDER	:= $(foreach dir,$(DATA),$(dir)/sprites)
export TST_FOLDER	:= $(foreach dir,$(DATA),$(dir)/tilesets)
export MAP_FOLDER	:= $(MAPS)
export FNT_FOLDER   := $(foreach dir,$(DATA),$(dir)/fonts)

# Tools
export TOOLS = $(CURDIR)/tools/tools

export OUTPUT	:=	$(CURDIR)/$(TARGET)

#export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
#					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(patsubst $(SOURCES)/%,%,$(call rwildcard,$(SOURCES)/,*.c))
CPPFILES	:=	$(patsubst $(SOURCES)/%,%,$(call rwildcard,$(SOURCES)/,*.cpp))
SFILES		:=	$(patsubst $(SOURCES)/%,%,$(call rwildcard,$(SOURCES)/,*.s))
# BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
ALLSRCS		:=  $(addprefix $(SOURCES)/, $(CFILES) $(CPPFILES) $(SFILES))

# Resources
SPRITES		:=  $(foreach dir,$(SPR_FOLDER),$(call rwildcard, $(dir)/,*.png))
TILESETS	:= 	$(foreach dir,$(TST_FOLDER),$(call rwildcard, $(dir)/,*.png))
MAPS		:=  $(foreach dir,$(MAP_FOLDER),$(wildcard $(dir)/*.tmx))
FONTS 		:=  $(foreach dir,$(FNT_FOLDER),$(call rwildcard, $(dir)/,*.ttf))
ALLRSRCS	:=	$(SPRITES) $(TILESETS) $(MAPS) $(FONTS)

export BUILDFOLDERS := $(addprefix $(CURDIR)/$(BUILD)/, $(sort $(dir $(ALLSRCS) $(ALLRSRCS))))
export VPATH 		:= $(CURDIR)/

ifneq ($(strip $(MUSIC)),)
	export AUDIOFILES	:=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.*)),$(CURDIR)/$(MUSIC)/$(dir))
	BINFILES += soundbank.bin
endif

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN := $(addsuffix .o,$(BINFILES))

export OFILES_SOURCES := $(addprefix $(SOURCES)/, $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o))

export OFILES_DATA := $(SPRITES:.png=.o) $(TILESETS:.png=.o) $(MAPS:.tmx=.o) $(MAPS:.tmx=_init.o) $(FONTS:.ttf=.o)

export OFILES := $(OFILES_BIN) $(OFILES_SOURCES) $(OFILES_DATA)
export DFILES := $(addprefix $(DEPSDIR)/, $(OFILES:.o=.d))

export HFILES_DATA := $(SPRITES:.png=.hpp) $(TILESETS:.png=.hpp) $(MAPS:.tmx=.hpp) $(FONTS:.ttf=.hpp)
export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES))) $(HFILES_DATA)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) -I$(CURDIR)/$(SOURCES)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@mkdir -p $(BUILDFOLDERS)
	@$(MAKE) -C tools
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	arm-none-eabi-objdump -dCS gba-modern.elf > gba-modern.dump

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@$(MAKE) --no-print-directory -C tools clean
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).gba

#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

$(OUTPUT).gba	:	$(OUTPUT).elf

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SOURCES) : $(HFILES)

$(OFILES_DATA) : $(TOOLS)

#---------------------------------------------------------------------------------
# The bin2o rule should be copied and modified
# for each extension used in the data directories
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
# rule to build soundbank from music files
#---------------------------------------------------------------------------------
soundbank.bin soundbank.h : $(AUDIOFILES)
#---------------------------------------------------------------------------------
	@mmutil $^ -osoundbank.bin -hsoundbank.h

#---------------------------------------------------------------------------------
# This rule links in binary data with the .bin extension
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
# Those rules link to ARM without LTO enabled for those objects only
#---------------------------------------------------------------------------------
%.niwram.o: %.niwram.cpp
	@echo $(notdir $<)
	$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.iwram.d $(CXXFLAGS) -fno-lto -marm -mlong-calls -c $< -o $@ $(ERROR_FILTER)

#---------------------------------------------------------------------------------
%.niwram.o: %.niwram.c
	@echo $(notdir $<)
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.iwram.d $(CFLAGS) -fno-lto -marm -mlong-calls -c $< -o $@ $(ERROR_FILTER)

#---------------------------------------------------------------------------------
# This rule is for sprites
#---------------------------------------------------------------------------------
$(SPR_FOLDER)/%.s $(SPR_FOLDER)/%.hpp: $(SPR_FOLDER)/%.png
	@echo $(notdir $<)
	$(TOOLS) sprite-export $< $(basename $@).s $(basename $@).hpp

#---------------------------------------------------------------------------------
# This rule is for tilesets
#---------------------------------------------------------------------------------
$(TST_FOLDER)/%.s $(TST_FOLDER)/%.hpp: $(TST_FOLDER)/%.png $(TST_FOLDER)/%.png.json
	@echo $(notdir $<)
	$(TOOLS) tileset-export $< $(basename $@).s $(basename $@).hpp

#---------------------------------------------------------------------------------
# This rule is for maps
#---------------------------------------------------------------------------------
$(MAP_FOLDER)/%.s $(MAP_FOLDER)/%_init.cpp $(MAP_FOLDER)/%.hpp: $(MAP_FOLDER)/%.tmx
	@echo $(notdir $<)
	$(eval MAP := $(basename $@))
	$(eval MAP := $(MAP:_init=))
	$(TOOLS) map-export $< $(MAP).s $(MAP)_init.cpp $(MAP).hpp

#---------------------------------------------------------------------------------
# This rule is for fonts
#---------------------------------------------------------------------------------
$(FNT_FOLDER)/%.s $(FNT_FOLDER)/%.hpp: $(FNT_FOLDER)/%.ttf $(FNT_FOLDER)/%.ttf.json
	@echo $(notdir $<)
	$(TOOLS) font-export $< $(basename $@).s $(basename $@).hpp

-include $(DFILES)
#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
