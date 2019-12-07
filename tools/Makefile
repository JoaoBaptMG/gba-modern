#------------------------------------------------------------------------------------------------
# Inside makefile to build the 'tools' target
#------------------------------------------------------------------------------------------------

# The parent Makefile will pass the ARM toolchain, we don't want it
CC				:= gcc
CXX				:= g++

C_FILES   		:= $(wildcard *.c)
CPP_FILES 		:= $(wildcard *.cpp)
O_FILES			:= $(C_FILES:%.c=obj/%.o) $(CPP_FILES:%.cpp=obj/%.o)

C_FLAGS		:= -Wall -g -I/usr/include/freetype2
CPP_FLAGS	:= $(C_FLAGS) -std=c++17
LIBS 		:= -lfreetype

PCH_SRC		:= pch.hpp
PCH_INC		:= grid.hpp lodepng.h nlohmann/json.hpp util.hpp
PCH_OUT		:= pch.hpp.gch

.PHONY: all clean

all: objdir tools

$(PCH_OUT): $(PCH_SRC) $(PCH_INC)
	$(CXX) $(CPP_FLAGS) -o $@ $<

-include obj/*.d

obj/%.o: %.c $(PCH_OUT)
	$(CC) -MMD $(C_FLAGS) -include $(PCH_SRC) -c -o $@ $<

obj/%.o: %.cpp $(PCH_OUT)
	$(CXX) -MMD $(CPP_FLAGS) -include $(PCH_SRC) -c -o $@ $<

tools: $(O_FILES)
	$(CXX) $(CPP_FLAGS) -o $@ $^ $(LIBS)

objdir:
	@mkdir -p obj

clean:
	@rm -rf obj
	@rm -f tools pch.hpp.gch
