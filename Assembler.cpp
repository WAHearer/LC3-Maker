#include"Assembler.h"
#define PCoffset(label,bits)({\
    if(link.find(label)==link.end()){\
        std::cout<<"Label \""<<label<<"\" does not exist.";\
        exit(1);\
    }\
    numToStrBin("#"+std::to_string(link[label]-pc),bits);\
})
const std::regex isNumber("X[+-]?[0-9A-F]+|#[+-]?[0-9]+");
const std::regex isValidSyntax("((ADD|AND) R[0-7],R[0-7],((R[0-7])|(X[+-]?[0-9A-F]+|#[+-]?[0-9]+)))|(BRN?Z?P? [^ ]+)|(JMP R[0-7])|(JSR [^ ]+)|(JSRR R[0-7])|((LD|LDI|LEA|ST|STI) R[0-7],[^ ]+)|((LDR|STR) R[0-7],R[0-7],(X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|(NOT R[0-7],R[0-7])|RET|RTI|(TRAP (X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|HALT|(\.ORIG (X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|(\.FILL (X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|(\.BLKW (X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|(\.STRINGZ \".+\")|\.END");
Assembler::Assembler(){
    oprandToBinary["ADD"]="0001";
    oprandToBinary["AND"]="0101";
    oprandToBinary["BR"]="0000";
    oprandToBinary["JMP"]="1100";
    oprandToBinary["JSR"]="0100";
    oprandToBinary["JSRR"]="0100";
    oprandToBinary["LD"]="0010";
    oprandToBinary["LDI"]="1010";
    oprandToBinary["LDR"]="0110";
    oprandToBinary["LEA"]="1110";
    oprandToBinary["NOT"]="1001";
    oprandToBinary["RET"]="1100";
    oprandToBinary["RTI"]="1000";
    oprandToBinary["ST"]="0011";
    oprandToBinary["STI"]="1011";
    oprandToBinary["STR"]="0111";
    oprandToBinary["TRAP"]="1111";
}
std::string Assembler::regNameToStrBin(const char &c){
    switch(c){
        case '0':
            return "000";
        case '1':
            return "001";
        case '2':
            return "010";
        case '3':
            return "011";
        case '4':
            return "100";
        case '5':
            return "101";
        case '6':
            return "110";
        case '7':
            return "111";
        default:
            std::cout<<"Invalid register name.";
            exit(1);
    }
}
long long Assembler::numToDex(const std::string &num){
    long long decimalNum=0;
    bool negative=false;
    if(num[0]=='X'){
        for(int i=1;i<num.length();i++){
            if(isdigit(num[i]))
                decimalNum=decimalNum*16+num[i]-'0';
            else if(num[i]>='A'&&num[i]<='F')
                decimalNum=decimalNum*16+num[i]-'A'+10;
            else if(num[i]=='-')
                negative=true;
        }
    }
    else if(num[0]=='#'){
        for(int i=1;i<num.length();i++){
            if(isdigit(num[i]))
                decimalNum=decimalNum*10+num[i]-'0';
            else if(num[i]=='-')
                negative=true;
        }
    }
    if(negative)
        decimalNum=-decimalNum;
    return decimalNum;
}
std::string Assembler::numToStrBin(const std::string &num, int bits){
    int decimalNum=0;
    bool negative=false;
    if(num[0]=='X'){
        for(int i=1;i<num.length();i++){
            if(isdigit(num[i]))
                decimalNum=decimalNum*16+num[i]-'0';
            else if(num[i]>='A'&&num[i]<='F')
                decimalNum=decimalNum*16+num[i]-'A'+10;
            else if(num[i]=='-')
                negative=true;
        }
    }
    else if(num[0]=='#'){
        for(int i=1;i<num.length();i++){
            if(isdigit(num[i]))
                decimalNum=decimalNum*10+num[i]-'0';
            else if(num[i]=='-')
                negative=true;
        }
    }
    if((negative&&decimalNum>1<<(bits-1))||(!negative&&decimalNum>(1<<(bits-1))-1)){
        std::cout<<"The number \""<<num<<"\" is too big.";
        exit(1);
    }
    if(negative&&decimalNum!=0)
        decimalNum=(1<<bits)-decimalNum;
    std::string binaryNum;
    for(int i=bits-1;i>=0;i--){
        if(1<<i<=decimalNum){
            binaryNum+="1";
            decimalNum-=1<<i;
        }
        else
            binaryNum+="0";
    }
    return binaryNum;
}
void Assembler::capitalize(std::string &code){
    for(auto &i:code){
        if(i>='a'&&i<='z')
            i=i-'a'+'A';
    }
}
void Assembler::deleteSpaceAndComment(std::string &code){
    int pos=0;
    std::string word;
    do{
        word="";
        while(pos<code.length()&&code[pos]==' ')
            code.erase(code.begin()+pos);
        if(pos>=code.length())
            return;
        while(pos<code.length()&&code[pos]!=' '){
            if(code[pos]=='"'){
                pos++;
                while(code[pos]!='"')
                    pos++;
                pos++;
            }
            if(code[pos]==';'){
                while(pos<code.length())
                    code.erase(code.begin()+pos);
                while(code[--pos]==' ')
                    code.erase(code.begin()+pos);
                return;
            }
            word+=code[pos];
            pos++;
        }
        if(pos>=code.length())
            return;
        pos++;
    }while(oprandToBinary[word].empty());
    while(pos<code.length()){
        if(code[pos]==';'){
            while(pos<code.length())
                code.erase(code.begin()+pos);
            while(code[--pos]==' ')
                code.erase(code.begin()+pos);
            return;
        }
        if(code[pos]!=' ')
            pos++;
        else{
            while(pos<code.length()&&code[pos]==' ')
                code.erase(code.begin()+pos);
        }
    }
}
void Assembler::cut(std::vector<std::string>&code){
    while(code[0].substr(0,5)!=".ORIG")
        code.erase(code.begin());
    int pos=0;
    while(code[pos]!=".END")
        if(code[pos].empty())
            code.erase(code.begin()+pos);
        else
            pos++;
    pos++;
    while(pos<code.size())
        code.erase(code.begin()+pos);
}
void Assembler::syntaxCheck(const std::vector<std::string>&code){
    int cnt=1;
    for(auto &i:code){
        if(!std::regex_search(i,isValidSyntax)){
            std::cout<<"Syntax error on line "<<cnt<<".";
            exit(1);
        }
        cnt++;
    }
}
std::map<std::string,long long>Assembler::linkLabel(const std::vector<std::string>&code){
    std::map<std::string,long long>link;
    long long pc=0;
    for(auto &i:code){
        std::istringstream iss(i);
        std::string word;
        iss>>word;
        if(word==".ORIG"){
            iss>>word;
            pc=numToDex(word);
        }
        else if(word==".BLKW"){
            iss>>word;
            pc+=numToDex(word);
        }
        else if(word==".STRINGZ"){
            iss>>word;
            pc+=word.length()-2;
        }
        else if(word==".FILL"){
            iss>>word;
            pc++;
        }
        else if(!oprandToBinary[word].empty()||word.substr(0,2)=="BR")
            pc++;
        else if(word==".END")
            pc=0;
        else{//is a label
            do{
                link[word]=pc;
                iss>>word;
            }while(!iss.eof()&&oprandToBinary[word].empty());
            if(!oprandToBinary[word].empty())
                pc++;
        }
    }
    return link;
}
std::vector<std::string>Assembler::assemble(std::vector<std::string>code){
    std::vector<std::string>binCode;
    for(auto &i:code){
        capitalize(i);
        deleteSpaceAndComment(i);
    }
    cut(code);
    syntaxCheck(code);
    std::map<std::string,long long>link=linkLabel(code);
    long long pc=0;
    for(auto &i:code){
        std::istringstream iss(i);
        std::string word;
        iss>>word;
        if(word==".ORIG"){
            iss>>word;
            pc=numToDex(word);
            binCode.push_back(numToStrBin(word,16));
        }
        else if(word==".BLKW"){
            iss>>word;
            pc+=numToDex(word);
        }
        else if(word==".STRINGZ"){
            iss>>word;
            pc+=word.length()-2;
        }
        else if(word==".FILL")
            pc++;
        else if(word==".END")
            pc=0;
        while(!iss.eof()&&oprandToBinary[word].empty()&&word.substr(0,2)!="BR"&&word!="HALT")
            iss>>word;
        if(word=="HALT"){
            pc++;
            binCode.push_back("1111000000100101");
            continue;
        }
        if(!oprandToBinary[word].empty()||word.substr(0,2)=="BR"){
            pc++;
            std::string binLine;
            if(!oprandToBinary[word].empty())
                binLine+=oprandToBinary[word];
            else
                binLine+="0000";
            if(word=="ADD"||word=="AND"){
                iss>>word;
                binLine+=regNameToStrBin(word[1]);
                binLine+=regNameToStrBin(word[4]);
                if(word[6]=='R'){
                    binLine+="000";
                    binLine+=regNameToStrBin(word[7]);
                }
                else{
                    binLine+="1";
                    binLine+=numToStrBin(word.substr(6),5);
                }
            }
            else if(word.substr(0,2)=="BR"){
                if(!word.find('N')&&!word.find('Z')&&!word.find('P'))
                    binLine+="111";
                else{
                    binLine+=word.find('N')!=std::string::npos?"1":"0";
                    binLine+=word.find('Z')!=std::string::npos?"1":"0";
                    binLine+=word.find('P')!=std::string::npos?"1":"0";
                }
                iss>>word;
                if(std::regex_match(word,isNumber))
                    binLine+=numToStrBin(word,9);
                else
                    binLine+=PCoffset(word,9);
            }
            else if(word=="JMP"){
                iss>>word;
                binLine+="000";
                binLine+=regNameToStrBin(word[1]);
                binLine+="000000";
            }
            else if(word=="JSR"){
                iss>>word;
                binLine+="1";
                if(std::regex_match(word,isNumber))
                    binLine+=numToStrBin(word,11);
                else
                    binLine+=PCoffset(word,11);
            }
            else if(word=="JSRR"){
                iss>>word;
                binLine+="000";
                binLine+=regNameToStrBin(word[1]);
                binLine+="000000";
            }
            else if(word=="LD"||word=="LDI"||word=="LEA"||word=="ST"||word=="STI"){
                iss>>word;
                binLine+=regNameToStrBin(word[1]);
                if(std::regex_match(word.substr(3),isNumber))
                    binLine+=numToStrBin(word.substr(3),9);
                else
                    binLine+=PCoffset(word.substr(3),9);
            }
            else if(word=="LDR"||word=="STR"){
                iss>>word;
                binLine+=regNameToStrBin(word[1]);
                binLine+=regNameToStrBin(word[4]);
                binLine+=numToStrBin(word,6);
            }
            else if(word=="NOT"){
                iss>>word;
                binLine+=regNameToStrBin(word[1]);
                binLine+=regNameToStrBin(word[4]);
                binLine+="111111";
            }
            else if(word=="RET"){
                binLine+="000111000000";
            }
            else if(word=="RTI"){
                binLine+="000000000000";
            }
            else if(word=="TRAP"){
                iss>>word;
                binLine+="0000";
                binLine+=numToStrBin(word,8);
            }
            binCode.push_back(binLine);
        }
    }
    return binCode;
}
