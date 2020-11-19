REM batch-file for first.gba
REM Note: you have to set the paths correctly first to make it work
REM Using makefiles is recommended.

PATH= e:\dev\msys\1.0\bin;e:\dev\devkitPro\devkitArm\bin;

arm-eabi-gcc -thum -mthumb-interworkb -c first.c
arm-eabi-gcc -mthumb -specs=gba_mb.specs -mthumb-interwork first.o -o first.elf
arm-eabi-objcopy -O binary first.elf first.gba
gbafix first.gba

pause

