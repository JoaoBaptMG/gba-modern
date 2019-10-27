#!/bin/bash

# Get the base name
bn="${1##*/}"
bn="${bn%.*}"

if [ -e $1.hpp ]
then
    echo "File $1.hpp already exists! I will not overwrite it!"
    exit 1
fi

if [ -e $1.cpp ]
then
    echo "File $1.cpp already exists! I will not overwrite it!"
    exit 1
fi

echo "//--------------------------------------------------------------------------------" > $1.hpp
echo "// $bn.hpp" >> $1.hpp
echo "//--------------------------------------------------------------------------------" >> $1.hpp
echo "// <insert description here>" >> $1.hpp
echo "//--------------------------------------------------------------------------------" >> $1.hpp
echo "" >> $1.hpp
echo "#pragma once" >> $1.hpp
echo "" >> $1.hpp

echo "//--------------------------------------------------------------------------------" > $1.cpp
echo "// $bn.cpp" >> $1.cpp
echo "//--------------------------------------------------------------------------------" >> $1.cpp
echo "// <insert description here>" >> $1.cpp
echo "//--------------------------------------------------------------------------------" >> $1.cpp
echo "" >> $1.cpp
echo "#include \"$bn.hpp\"" >> $1.cpp
echo "" >> $1.cpp
