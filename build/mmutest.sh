#!/bin/bash
cd ..
clear
clear
make clean
make testmmu
cd build
./testmmu.exe
