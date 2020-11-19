set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER ${CMAKE_SOURCE_DIR}/gcc/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${CMAKE_SOURCE_DIR}/gcc/bin/arm-none-eabi-g++)
set(CMAKE_OBJCOPY ${CMAKE_SOURCE_DIR}/gcc/bin/arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP ${CMAKE_SOURCE_DIR}/gcc/bin/arm-none-eabi-objdump)