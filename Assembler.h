#include<string>
#include<vector>
#include<map>
#include<sstream>
#include<regex>
#include<iostream>
#ifndef LC3_ASSEMBLER_ASSEMBLER_H
#define LC3_ASSEMBLER_ASSEMBLER_H
class Assembler{
private:
    std::map<std::string,std::string>oprandToBinary;
    static std::string regNameToStrBin(const char &c);//translate register names to 3-bit binary
    static long long numToDex(const std::string &num);//translate decimal or hexadecimal number to long long decimal number
    static std::string numToStrBin(const std::string &num,int bits);//translate decimal or hexadecimal number to 2's complement
    void capitalize(std::string &code);//capitalize all letters
    void deleteSpaceAndComment(std::string &code);//delete unnecessary spaces and comments
    static void cut(std::vector<std::string>&code);//cut the program from .ORIG to .END and delete blank line
    static void syntaxCheck(const std::vector<std::string>&code);
    std::map<std::string,long long>linkLabel(const std::vector<std::string>&code);//translate labels to addresses
public:
    Assembler();
    std::vector<std::string>assemble(std::vector<std::string>code);
};
#endif
