cmake_minimum_required(VERSION 3.29)
project(LC3_Assembler)

set(CMAKE_CXX_STANDARD 20)

add_executable(LC3_Assembler main.cpp
        Assembler.cpp
        Assembler.h
        Simulator.cpp
        Simulator.h
)
