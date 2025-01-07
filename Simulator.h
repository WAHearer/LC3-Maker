#include<iostream>
#include<cstring>
#include<vector>
#include<conio.h>
#include<thread>
#ifndef LC3_MAKER_SIMULATOR_H
#define LC3_MAKER_SIMULATOR_H
class Simulator{
private:
    short memory[65536],reg[8];
    bool PSR[16];
    unsigned short pc,SAVED_SSP,SAVED_USP;
    short binToNum(const std::string &num);//transform 16-bit binary to decimal
    std::string numToBin(short num);//transform decimal to 16-bit binary
public:
    Simulator();
    void load(std::vector<std::string>&code);
    void run();
};
#endif