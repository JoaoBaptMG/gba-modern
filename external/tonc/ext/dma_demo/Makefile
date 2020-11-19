#
# dma_demo.mak
#
# makefile for HDMA demonstration
#
# NOTE: for educational purposes only. For real work, use 
# devkitPro's templates ( $(DEVKITPRO)/examples/gba/template )
# or tonclib's templates ( $(TONCCODE)/lab/template )


PATH := $(DEVKITARM)/bin:$(PATH)

# --- Project details -------------------------------------------------

PROJ    := dma_demo
TITLE   := $(PROJ)

LIBTONC	:= ../../tonclib

INCLUDE  := -I$(LIBTONC)/include
LIBPATHS := -L$(LIBTONC)/lib

LIBS    := -ltonc

# ROM, IWRAM, asm sources
RCSRC   := $(PROJ).c brin.c
ICSRC   := 
SSRC    := 

# boot mode: cart/multiboot
bMB     := 0
# Generate asm for C files 
bGENASM := 0
# generate map file
bMAP    := 0

# === The rest need not be altered (probably) =========================

CSRC    := $(RCSRC) $(ICSRC)

RCOBJ   := $(RCSRC:.c=.o)
ICOBJ   := $(ICSRC:.c=.o)
COBJ    := $(RCOBJ) $(ICOBJ)
SOBJ    := $(SSRC:.s=.o)

OBJ     := $(COBJ) $(SOBJ)

ifeq ($(strip $(bMB)), 1)
TARGET  := $(PROJ).mb
SPECS   := -specs=gba_mb.specs
else
TARGET  := $(PROJ)
SPECS   := -specs=gba.specs
endif

# --- Compiling -------------------------------------------------------

CROSS   ?= arm-none-eabi-
AS      := $(CROSS)gcc
CC      := $(CROSS)gcc
LD      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy

ARCH    := -mthumb-interwork -mthumb
RARCH   := -mthumb-interwork -mthumb
IARCH   := -mthumb-interwork -marm

CBASE   := $(INCLUDE) -O2 -Wall -fno-strict-aliasing
RCFLAGS := $(CBASE) $(RARCH)
ICFLAGS := $(CBASE) $(IARCH) -mlong-calls
CFLAGS  := $(RCFLAGS)

ASFLAGS := -x assembler-with-cpp  -c -mthumb-interwork
LDFLAGS := $(ARCH) $(SPECS) $(LIBPATHS) $(LIBS)

ifeq ($(strip $(bMAP)), 1)
	LDFLAGS += -Wl,-Map,$(PROJ).map
endif

# === TARGETS =========================================================

.PHONY: build clean

# --- Build -----------------------------------------------------------

# --- Main target ---
build: depends $(TARGET).gba
	@echo done

# --- Translation ---
# ---.elf -> .gba ---
$(TARGET).gba : $(TARGET).elf
	@$(OBJCOPY) -v -O binary $< $@
	-@gbafix $@ -t$(TITLE)

# --- Linking -------------
# --- *.o *.a -> .elf -----
$(TARGET).elf : $(OBJ) 
	$(LD) $^ $(LDFLAGS) -o $@

# --- ROM compilation ---
# --- *.c -> *.o --------
$(RCOBJ) : %.o : %.c
	$(CC) $(RCFLAGS) -c $< -o $@
ifeq ($(strip $(bGENASM)), 1)
	$(CC) $(RCFLAGS) -fverbose-asm -S $<
endif

# --- IWRAM compilation ----
# *.iwram.c -> *.iwram.o ---
$(ICOBJ) : %.iwram.o : %.iwram.c
	$(CC) $(ICFLAGS) -c $< -o $@
ifeq ($(strip $(bGENASM)), 1)
	$(CC) $(ICFLAGS) -fverbose-asm -S $<
endif

# --- Assembling ---
# --- *.s -> *.o ---
$(SOBJ) : %.o : %.s
	$(AS) $(ASFLAGS) -c $< -o $@

# --- Header dependencies ---
# --- *.c -> .d -------------
depends:
	$(CC) -MM $(CFLAGS) -c $(ICSRC) $(RCSRC) > $(PROJ).d

-include $(PROJ).d

# --- Build Lib -------------------------------------------------------

buildlib : 
	$(MAKE) -C $(UDIR) -f tonclib.mak	buildlib

# --- Clean -----------------------------------------------------------

clean : 
	@rm -fv $(TARGET).gba
	@rm -fv $(TARGET).elf $(PROJ).map $(PROJ).d
	@rm -fv $(CSRC:.c=.s)
	@rm -fv $(OBJ)

#EOF
