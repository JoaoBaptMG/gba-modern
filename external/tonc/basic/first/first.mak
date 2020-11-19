#
# first.mak
#
# a minimal makefile (and I do mean minimal, look to the others
# for real-life use)

PATH   := $(DEVKITARM)/bin:$(PATH)

first.gba : first.c
	arm-none-eabi-gcc -mthumb -mthumb-interwork -c first.c
	arm-none-eabi-gcc -specs=gba.specs -mthumb -mthumb-interwork first.o -o first.elf
	arm-none-eabi-objcopy -v -O binary first.elf first.gba
	gbafix first.gba

#EOF
