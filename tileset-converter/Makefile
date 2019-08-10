#------------------------------------------------------------------------------------------------
# Inside makefile to build the 'tileset-converter'
#------------------------------------------------------------------------------------------------

# The parent Makefile will pass the ARM toolchain, we don't want it
CC				:= gcc
CXX				:= g++

C_FILES   		:= $(wildcard *.c)
CPP_FILES 		:= $(wildcard *.cpp)
O_FILES			:= $(C_FILES:%.c=obj/%.o) $(CPP_FILES:%.cpp=obj/%.o)

INC_FOLDER := ../tools

C_FLAGS		:= -I$(INC_FOLDER) -Wall -g
CPP_FLAGS	:= $(C_FLAGS) -std=c++17

.PHONY: all clean

all: objdir tileset-converter

-include obj/*.d

obj/%.o: %.c
	$(CC) -MMD $(C_FLAGS) -c -o $@ $<

obj/%.o: %.cpp
	$(CXX) -MMD $(CPP_FLAGS) -c -o $@ $<

tileset-converter: $(O_FILES)
	$(CXX) $(CPP_FLAGS) -o $@ $^

objdir:
	@mkdir -p obj

clean:
	@rm -rf obj
	@rm -f tileset-converter
