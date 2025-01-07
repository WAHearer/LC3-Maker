#include<iostream>
#include<cstring>
#include<vector>
#include<map>
#include<sstream>
#include<regex>
#ifndef LC3_MAKER_ASSEMBLER_H
#define LC3_MAKER_ASSEMBLER_H
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
#endif