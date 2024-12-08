#include<iostream>
#include<vector>
#include"Assembler.h"
using std::cin,std::cout,std::endl;
int main(){
    Assembler assembler;
    freopen("code.txt","r",stdin);
    std::vector<std::string>code;
    std::string strInput;
    while(std::getline(cin,strInput)){
        code.push_back(strInput);
    }
    std::vector<std::string>binCode=assembler.assemble(code);
    Runner runner;
    runner.run(binCode);
    return 0;
}