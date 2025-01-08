#include<iostream>
#include<vector>
#include<fstream>
#include"Assembler.h"
#include"Simulator.h"
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
    Simulator simulator;
    simulator.load(binCode);
    simulator.run();
    system("pause");
    return 0;
}