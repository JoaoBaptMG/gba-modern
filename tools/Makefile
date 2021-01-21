#------------------------------------------------------------------------------------------------
# Inside makefile to build the 'tools' target
#------------------------------------------------------------------------------------------------

# The parent Makefile will pass the ARM toolchain, we don't want it
CC				:= gcc
CXX				:= g++

C_FILES   		:= $(wildcard *.c)
CPP_FILES 		:= $(wildcard *.cpp)
O_FILES			:= $(C_FILES:%.c=obj/%.o) $(CPP_FILES:%.cpp=obj/%.o)

C_FLAGS		:= -Wall -g -I/usr/include/freetype2 -I../libsamplerate-install/include
CPP_FLAGS	:= $(C_FLAGS) -std=c++17
LIBS 		:= -lfreetype ../libsamplerate-install/lib/libsamplerate.a

PCH_SRC		:= pch.hpp
PCH_INC		:= grid.hpp lodepng.h nlohmann/json.hpp util.hpp
PCH_OUT		:= pch.hpp.gch

LSRC_INSTALL_FOLDER = $(dir $(CURDIR))libsamplerate-install

.PHONY: all clean

all: tools

$(PCH_OUT): $(PCH_SRC) $(PCH_INC)
	$(CXX) $(CPP_FLAGS) -o $@ $<

-include obj/*.d

obj/%.o: %.c $(PCH_OUT) | $(LSRC_INSTALL_FOLDER)
	@mkdir -p obj
	$(CC) -MMD $(C_FLAGS) -include $(PCH_SRC) -c -o $@ $<

obj/%.o: %.cpp $(PCH_OUT) | $(LSRC_INSTALL_FOLDER)
	@mkdir -p obj
	$(CXX) -MMD $(CPP_FLAGS) -include $(PCH_SRC) -c -o $@ $<

tools: $(LSRC_INSTALL_FOLDER) $(O_FILES)
	$(CXX) $(CPP_FLAGS) -o $@ $(O_FILES) $(LIBS)

$(LSRC_INSTALL_FOLDER): ../libsamplerate
	cd ../libsamplerate && ./configure --prefix $(LSRC_INSTALL_FOLDER)
	$(MAKE) -C ../libsamplerate
	$(MAKE) -C ../libsamplerate install

clean:
	-$(MAKE) -C ../libsamplerate distclean
	@rm -rf $(LSRC_INSTALL_FOLDER)
	@rm -rf obj
	@rm -f tools pch.hpp.gch
