#! /bin/sh
txtblk=$(tput setaf 0) # Black - Regular
txtred=$(tput setaf 1) # Red
txtgrn=$(tput setaf 2) # Green
txtylw=$(tput setaf 3) # Yellow
txtblu=$(tput setaf 4) # Blue
txtpur=$(tput setaf 5) # Purple
txtcyn=$(tput setaf 6) # Cyan
txtwht=$(tput setaf 7) # White

# Break if any command fails
set -e 
set -o pipefail

echo -e "${txtgrn}[1/3] Converting CMake to Makefile ${txtwht}"
mkdir -p bin
cd bin/
cmake -DCMAKE_BUILD_TYPE=Release ../CMakeLists.txt

echo -e "${txtgrn}[2/3] Compiling DA2324_PRJ2_G163 ${txtwht}"
make -j$(nproc)

cd ..
clear

echo -e "${txtgrn}[3/3] Executing DA2324_PRJ2_G163 ${txtwht}"
sh -c "./bin/DA2324_PRJ2_G163 $1 $2"

