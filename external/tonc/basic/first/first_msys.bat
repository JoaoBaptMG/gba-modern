
#
# you can also try running in an MSYS box, which is 
# better than a DOS box, but setting the path is a little different
# use 
#   PATH=[path1:path2]. 
#     separate the list by colons ':'
#     and _no_ spaces around the '='!!
# 
# Comment with '#', obviously, and pasting uses the middle-mouse button.
# an MSYS box also allows scrolling and moving around by arrows
#

PATH   := $(DEVKITARM)/bin:$(PATH)

arm-elf-gcc -mthumb -mthumb-interwork -c first.c
arm-elf-gcc -specs=gba_mb.specs -mthumb -mthumb-interwork first.o -o first.elf
arm-elf-objcopy -O binary first.elf first.gba
gbafix first.gba


