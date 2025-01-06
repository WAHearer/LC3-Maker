#include<iostream>
#include<fstream>
#include<vector>
#include"Assembler.h"
using std::cin,std::cout,std::endl;
int main(){
    setbuf(stdout, 0);
    Assembler assembler;
    std::ifstream ifs("code.txt");
    std::vector<std::string>code;
    std::string strInput;
    while(std::getline(ifs,strInput)){
        code.push_back(strInput);
    }
    std::vector<std::string>binCode=assembler.assemble(code);
    Runner runner;
    runner.run(binCode);
    return 0;
}