#include<cstring>
#include<vector>
#include<map>
#include<sstream>
#include<regex>
#include<iostream>
#include<conio.h>
#include<thread>
#include<functional>
#ifndef LC3_ASSEMBLER_ASSEMBLER_H
#define LC3_ASSEMBLER_ASSEMBLER_H
class Assembler{
private:
    std::map<std::string,std::string>oprandToBinary;
    std::map<std::string,bool>isPseudoInstruction;
    std::string regNameToStrBin(const char &c);//generate 3-bit register name
    int numToDex(const std::string &num);//transform string type decimal or hexadecimal number to int type decimal number
    std::string numToStrBin(const std::string &num,int bits);//transform decimal or hexadecimal number to 2's complement
    void capitalize(std::vector<std::string>&code);//capitalize all letters and replace tab with space
    void deleteSpaceAndComment(std::vector<std::string>&code);//delete unnecessary space and comment
    void cut(std::vector<std::string>&code);//cut the program from .ORIG to .END
    void syntaxCheck(const std::vector<std::string>&code);//check the syntax of the program
    std::map<std::string,int>linkLabel(const std::vector<std::string>&code);//link label with address
public:
    Assembler();
    std::vector<std::string>assemble(std::vector<std::string>code);
};
class Runner{
private:
    short memory[65536],reg[8];
    bool PSR[16];
    unsigned short pc,SAVED_SSP,SAVED_USP;
    short binToNum(const std::string &num);//transform 16-bit binary to decimal
    std::string numToBin(short num);//transform decimal to 16-bit binary
    void load(std::vector<std::string>&code);
public:
    Runner();
    void run(std::vector<std::string>code);
};
#endif