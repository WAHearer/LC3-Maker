#include"Assembler.h"
#define PCoffset(label,bits)({\
    if(link.find(label)==link.end()){\
        std::cout<<"Label \""<<label<<"\" does not exist.";\
        exit(1);\
    }\
    numToStrBin("#"+std::to_string(link[label]-pc),bits);\
})
const std::regex isNumber("X[+-]?[0-9A-F]+|#[+-]?[0-9]+");
const std::regex isValidSyntax("((ADD|AND) R[0-7],R[0-7],((R[0-7])|(X[+-]?[0-9A-F]+|#[+-]?[0-9]+)))|(BRN?Z?P? [^ ]+)|(JMP R[0-7])|(JSR [^ ]+)|(JSRR R[0-7])|((LD|LDI|LEA|ST|STI) R[0-7],[^ ]+)|((LDR|STR) R[0-7],R[0-7],(X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|(NOT R[0-7],R[0-7])|RET|RTI|(TRAP (X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|GETC|OUT|PUTS|IN|PUTSP|HALT|(\.ORIG (X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|(\.FILL [^ ]+)|(\.BLKW (X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|(\.STRINGZ \".+\")|\.END");
Assembler::Assembler(){
    oprandToBinary["ADD"]="0001";
    oprandToBinary["AND"]="0101";
    oprandToBinary["BR"]="0000";
    oprandToBinary["BRN"]="0000";
    oprandToBinary["BRZ"]="0000";
    oprandToBinary["BRP"]="0000";
    oprandToBinary["BRNZ"]="0000";
    oprandToBinary["BRNP"]="0000";
    oprandToBinary["BRZP"]="0000";
    oprandToBinary["BRNZP"]="0000";
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
    oprandToBinary["GETC"]="1111";
    oprandToBinary["OUT"]="1111";
    oprandToBinary["PUTS"]="1111";
    oprandToBinary["IN"]="1111";
    oprandToBinary["PUTSP"]="1111";
    oprandToBinary["HALT"]="1111";
    isPseudoInstruction[".ORIG"]=true;
    isPseudoInstruction[".FILL"]=true;
    isPseudoInstruction[".BLKW"]=true;
    isPseudoInstruction[".STRINGZ"]=true;
    isPseudoInstruction[".END"]=true;
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
int Assembler::numToDex(const std::string &num){
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
void Assembler::capitalize(std::vector<std::string>&code){
    for(auto &line:code){
        bool inStr=0;
        for(auto &i:line){
            if(i=='\"')
                inStr=!inStr;
            if(i>='a'&&i<='z'&&!inStr)
                i=i-'a'+'A';
            else if(i==9)//tab
                i=' ';
        }
    }
}
void Assembler::deleteSpaceAndComment(std::vector<std::string>&code){
    for(auto &line:code){
        int pos=0;
        std::string word;
        do{
            word="";
            while(pos<line.length()&&line[pos]==' ')
                line.erase(line.begin()+pos);
            if(pos>=line.length())
                break;
            while(pos<line.length()&&line[pos]!=' '){
                if(line[pos]=='"'){
                    pos++;
                    while(line[pos]!='"')
                        pos++;
                    pos++;
                }
                if(line[pos]==';'){
                    while(pos<line.length())
                        line.erase(line.begin()+pos);
                    while(line[--pos]==' ')
                        line.erase(line.begin()+pos);
                    break;
                }
                word+=line[pos];
                pos++;
            }
            if(pos>=line.length())
                break;
            pos++;
        }while(oprandToBinary[word].empty());
        while(pos<line.length()){
            if(line[pos]==';'){
                while(pos<line.length())
                    line.erase(line.begin()+pos);
                while(line[--pos]==' ')
                    line.erase(line.begin()+pos);
                break;
            }
            if(line[pos]!=' ')
                pos++;
            else{
                while(pos<line.length()&&line[pos]==' ')
                    line.erase(line.begin()+pos);
            }
        }
    }
}
void Assembler::cut(std::vector<std::string>&code){
    bool start=0,end=0;
    for(auto &line:code){
        if(line.substr(0,5)==".ORIG"){
            if(!start)
                start=1;
            else{
                std::cout<<"error:multiple .ORIG instructions";
                exit(1);
            }
        }
        else if(line==".END"){
            if(!end)
                end=1;
            else{
                std::cout<<"error:multiple .END instructions";
                exit(1);
            }
        }
        else{
            if(!start||end)
                line="";
        }
    }
}
void Assembler::syntaxCheck(const std::vector<std::string>&code){
    int cnt=0;
    for(auto &i:code){
        cnt++;
        std::istringstream iss(i);
        std::string word;
        bool flag=0;
        while(!iss.eof()){
            iss>>word;
            if(!oprandToBinary[word].empty()||isPseudoInstruction[word]){
                flag=1;
                break;
            }
        }
        if(!flag)//only labels
            continue;
        if(!std::regex_search(i,isValidSyntax)){
            std::cout<<"Syntax error on line "<<cnt<<".";
            exit(1);
        }
    }
}
std::map<std::string,int>Assembler::linkLabel(const std::vector<std::string>&code){
    std::map<std::string,int>link;
    int pc=0;
    for(auto &i:code){
        std::istringstream iss(i);
        std::string word;
        iss>>word;
        while(!iss.eof()&&oprandToBinary[word].empty()&&!isPseudoInstruction[word]){
            link[word]=pc;
            iss>>word;
        }
        if(oprandToBinary[word].empty()&&!isPseudoInstruction[word])
            link[word]=pc;
        else if(!oprandToBinary[word].empty())
            pc++;
        else if(isPseudoInstruction[word]){
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
        }
    }
    return link;
}
std::vector<std::string>Assembler::assemble(std::vector<std::string>code){
    std::vector<std::string>binCode;
    capitalize(code);
    deleteSpaceAndComment(code);
    cut(code);
    syntaxCheck(code);
    std::map<std::string,int>link=std::move(linkLabel(code));
    int pc=0;
    for(auto &i:code){
        std::istringstream iss(i);
        std::string word;
        do{
            iss>>word;
        }while(!iss.eof()&&oprandToBinary[word].empty()&&!isPseudoInstruction[word]);
        if(!oprandToBinary[word].empty()){
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
                if(word.find('N')==std::string::npos&&word.find('Z')==std::string::npos&&word.find('P')==std::string::npos)
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
                binLine+=numToStrBin(word.substr(6),6);
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
            else if(word=="GETC"){
                binLine+="000000100000";
            }
            else if(word=="OUT"){
                binLine+="000000100001";
            }
            else if(word=="PUTS"){
                binLine+="000000100010";
            }
            else if(word=="IN"){
                binLine+="000000100011";
            }
            else if(word=="PUTSP"){
                binLine+="000000100100";
            }
            else if(word=="HALT"){
                binLine+="000000100101";
            }
            binCode.push_back(binLine);
        }
        else if(word==".ORIG"){
            iss>>word;
            pc=numToDex(word);
            binCode.push_back(numToStrBin(word,16));
        }
        else if(word==".BLKW"){
            iss>>word;
            int length=numToDex(word);
            for(int j=1;j<=length;j++)
                binCode.push_back("0000000000000000");
            pc+=length;
        }
        else if(word==".STRINGZ"){
            std::getline(iss,word);
            int pos=0;
            while(word[pos]!='\"')
                pos++;
            pos++;
            for(;word[pos]!='\"';pos++)
                binCode.push_back(numToStrBin('#'+std::to_string(int(word[pos])),16));
            pc+=word.length()-2;
        }
        else if(word==".FILL"){
            iss>>word;
            if(std::regex_match(word,isNumber))
                binCode.push_back(numToStrBin(word,16));
            else{
                if(link.find(word)==link.end()){
                    std::cout<<"Label \""<<word<<"\" does not exist.";
                    exit(1);
                }
                else
                    binCode.push_back(numToStrBin("#"+std::to_string(link[word]),16));
            }
            pc++;
        }
        else if(word==".END")
            pc=0;
    }
    return binCode;
}

short Runner::binToNum(const std::string &num){
    short decimalNum=0;
    for(auto &i:num){
        decimalNum=decimalNum*2+i-'0';
    }
    if(decimalNum>=(1<<(num.length()-1)))
        decimalNum-=(1<<num.length());
    return decimalNum;
}
std::string Runner::numToBin(short num){
    std::string binaryNum;
    unsigned short unsignedNum=(num>=0)?num:num+65536;
    for(int i=15;i>=0;i--){
        if(unsignedNum>=(1<<i)){
            unsignedNum-=(1<<i);
            binaryNum+='1';
        }
        else
            binaryNum+='0';
    }
    return binaryNum;
}
void Runner::run(std::vector<std::string>code){
    short *r=new short[8];
    short *memory=new short[65535];
    unsigned short basic=(unsigned short)binToNum(code[0]);
    bool n=false,z=false,p=false;
    for(int i=0;i<code.size();i++){
        memory[basic+i-1]=binToNum(code[i]);//load code into memory
    }
    unsigned short pc=basic;
    while(true){
        std::string line=numToBin(memory[pc]);
        pc++;
        std::string op=line.substr(0,4);
        short &dr=r[binToNum(line.substr(4,3))];
        short &sr1=r[binToNum(line.substr(7,3))];
        short &sr2=r[binToNum(line.substr(13,3))];
        short &baser=r[binToNum(line.substr(7,3))];
        short imm5=binToNum(line.substr(11,5));
        short offset6=binToNum(line.substr(10,6));
        short offset9=binToNum(line.substr(7,9));
        short offset11=binToNum(line.substr(5,11));
        short trapvect8=binToNum(line.substr(8,8));
        if(op=="0001"){//ADD
            if(line[10]=='0')
                dr=sr1+sr2;
            else
                dr=sr1+imm5;
            n=(dr<0);
            z=(dr==0);
            p=(dr>0);
        }
        else if(op=="0101"){//AND
            if(line[10]=='0')
                dr=sr1&sr2;
            else
                dr=sr1&imm5;
            n=(dr<0);
            z=(dr==0);
            p=(dr>0);
        }
        else if(op=="0000"){//BR
            if((line[4]=='1'&&n)||(line[5]=='1'&&z)||(line[6]=='1'&&p))
                pc+=offset9;
        }
        else if(op=="1100"){//JMP RET
            pc=baser;
        }
        else if(op=="0100"){//JSR JSRR
            r[7]=pc;
            if(line[4]=='1')
                pc+=offset11;
            else
                pc+=baser;
        }
        else if(op=="0010"){//LD
            dr=memory[pc+offset9];
            n=(dr<0);
            z=(dr==0);
            p=(dr>0);
        }
        else if(op=="1010"){//LDI
            dr=memory[(unsigned short)memory[pc+offset9]];
            n=(dr<0);
            z=(dr==0);
            p=(dr>0);
        }
        else if(op=="0110"){//LDR
            dr=memory[(unsigned short)baser+offset6];
            n=(dr<0);
            z=(dr==0);
            p=(dr>0);
        }
        else if(op=="1110"){//LEA
            dr=pc+offset9;
        }
        else if(op=="1001"){//NOT
            dr=(short)~sr1;
            n=(dr<0);
            z=(dr==0);
            p=(dr>0);
        }
        else if(op=="1100"){//RET
            pc=r[7];
        }
        else if(op=="0011"){//ST
            memory[pc+offset9]=dr;
        }
        else if(op=="1011"){//STI
            memory[(unsigned short)memory[pc+offset9]]=dr;
        }
        else if(op=="0111"){//STR
            memory[(unsigned short)baser+offset6]=dr;
        }
        else if(op=="1111"){//TRAP
            switch(trapvect8){
                case(0x20)://GETC
                    r[0]=getch();
                    break;
                case(0x21)://OUT
                    std::cout<<(char)r[0]<<std::endl;
                    break;
                case(0x22)://PUTS
                    for(unsigned short i=r[0];memory[i];i++){
                        std::cout<<(char)memory[i];
                    }
                    std::cout<<std::endl;
                    break;
                case(0x23)://IN
                    std::cout<<"Input a character:";
                    r[0]=getchar();
                    break;
                case(0x24)://PUTSP
                    for(unsigned short i=r[0];;i++){
                        if((char)memory[i]==0)
                            break;
                        std::cout<<(char)memory[i];
                        if((char)(memory[i]>>8)==0)
                            break;
                        std::cout<<(char)(memory[i]>>8);
                    }
                    std::cout<<std::endl;
                    break;
                case(0x25)://HALT
                    std::cout<<"Program halted";
                    return;
            }
        }
    }
}