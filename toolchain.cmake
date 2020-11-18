set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Path to the tools used
set(ARMCC ${CMAKE_SOURCE_DIR}/gcc/bin/arm-none-eabi-gcc CACHE PATH "ARMCC")
#ARMCC := gcc/bin/arm-none-eabi-gcc
set(ARMCPP ${CMAKE_SOURCE_DIR}/gcc/bin/arm-none-eabi-g++ CACHE PATH "ARMCPP")
#ARMCPP := gcc/bin/arm-none-eabi-g++
set(ARMOC ${CMAKE_SOURCE_DIR}/gcc/bin/arm-none-eabi-objcopy CACHE PATH "ARMOC")
#ARMOC := gcc/bin/arm-none-eabi-objcopy
set(ARMOD ${CMAKE_SOURCE_DIR}/gcc/bin/arm-none-eabi-objdump CACHE PATH "ARMOD")
#ARMOD := gcc/bin/arm-none-eabi-objdump
set(ARMLD ${ARMCPP})

set(CMAKE_C_COMPILER ${ARMCC})
set(CMAKE_CXX_COMPILER ${ARMCPP})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)