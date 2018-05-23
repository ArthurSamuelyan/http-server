#!/bin/bash
#=== This script compiles boost.asio apps. ===

# TODO: Alter this script:
# It should walk through the directory ./Src and compile all .cpp files
# and put a single binary, named $1, into ./Bin.
# Perhaps we should use ./Build directory to put .o files in there.
# The script should also create the missing directories.

libNames=\
"boost_system \
boost_thread \
boost_date_time \
boost_regex \
boost_serialization"

includePath="/usr/local/lib/boost/include"
libDirPath="/usr/local/lib/boost/lib/"

libs=""
for name in $libNames
do
libs=$libs" "$libDirPath"lib"$name".a"
done

compiler=c++
cFlags=" -c -std=c++11 -I "$includePath
lFlags=" -lpthread "$libs

#c++ -I $includePath $sourceFile -o $targetFile -L$libDirPath $libs 
#This compiles with no warnings, but the compiled program cannot find the lib and terminates"

rm Build/HTTPServer/*.o
for sourceFile in Src/HTTPServer/*.cpp
do
objectFile=${sourceFile//".cpp"/".o"}
objectFile=${objectFile//"Src/HTTPServer/"/"Build/HTTPServer/"}
#echo "=========Compiling "$sourceFile
echo $compiler$cFlags $sourceFile -o $objectFile
$compiler$cFlags $sourceFile -o $objectFile
done
echo "==================================== Compilation done =========================================="

objectFiles=""
for currObjectFile in Build/HTTPServer/*.o
do
objectFiles=$objectFiles" "$currObjectFile
done

#$compiler $objectFiles -o Bin/$1$lFlags
$compiler $objectFiles -o Bin/HTTPServer$lFlags
echo "====================================== Linking done ============================================"

