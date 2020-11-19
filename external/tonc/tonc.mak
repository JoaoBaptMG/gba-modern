#
# Hub makefile for Tonc's demos
#

# === Variables =======================================================

export CROSS	:= arm-none-eabi-

LIBTONC		:= tonclib
GRPDIRS		:= basic ext adv lab
BINDIR		:= ../bin

# List all directories in the group dirs
# Yeah, there's probably a better way :/
PROJALL		:= $(foreach dir, $(GRPDIRS), $(shell ls -d $(dir)/*/))
EXCLUDE		:= 
PROJDIRS	:= $(filter-out $(foreach proj, $(EXCLUDE), %/$(proj)/), $(PROJALL))

DEMO	:= bigmap
DEMODIR	:= $(filter %/$(DEMO)/, $(PROJDIRS))

# Convert GCC lines to conform to the right IDE, feel free to add more
# GCC : {filename}:{line}: ...
# MSVS: {dir}\{filename}({line}): ...
ifeq ($(IDE),msvc)

# And switch is DEMO is not in 'basic' or 'ext', 
# as the rest behaves differently :/
ifeq ($(filter basic/% ext/%, lab/%, $(DEMODIR)),)
	ERRFIX  := 2>&1 | sed -e 's|\(.\+\):\([0-9]\+\):|\1(\2):|'
else
	ERRFIX  := 2>&1 | sed -e 's|\(.\+\):\([0-9]\+\):|../$(DEMODIR)\1(\2):|'
endif

endif	# /$(IDE)

.PHONY : build clean build_all clean_all 

# === Build Single ====================================================

build:
	$(MAKE) -C $(DEMODIR) $(ERRFIX)
	

# === Clean Single ====================================================

clean:
	@$(MAKE) -C $(DEMODIR) clean


# === Build Single ====================================================

build_all: libtonc
	@echo === Building all projects \(this could take a while\) ===
	@for proj in $(PROJDIRS) ;				\
	    do $(MAKE) -s -C $$proj	; done;
	@[ -d $(BINDIR) ] || mkdir -p $(BINDIR)
	@for proj in $(PROJDIRS); do cp $$proj*.gba $(BINDIR) ; done;

# === Clean Single ====================================================

clean_all:
	@echo === Cleaning all projects \(this could take a while\) ===
#	@$(MAKE) clean -C $(LIBTONC)
	@for proj in $(PROJDIRS);				\
		do $(MAKE) clean -s -C $$proj ; done;


# === Build libtonc ===================================================

libtonc:
	@$(MAKE) -C $(LIBTONC)
	
