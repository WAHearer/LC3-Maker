#include"Assembler.h"
#define PCoffset(label,bits)({\
    if(link.find(label)==link.end()){\
        std::cout<<"Label \""<<label<<"\" does not exist.";\
        exit(1);\
    }\
    numToStrBin("#"+std::to_string(link[label]-pc),bits);\
})
#define check(pc)({\
    if(mode&&(pc<0x3000||pc>=0xFE00)){\
        std::cout<<"error:memory "<<pc<<" is not accessible";\
        exit(1);\
    }\
})
#define mode PSR[15]
#define PL ((PSR[10]<<2)+(PSR[9]<<1)+PSR[8])
#define n PSR[2]
#define z PSR[1]
#define p PSR[0]
#define KBSR memory[0xFE00]
#define KBDR memory[0xFE02]
#define DSR memory[0xFE04]
#define DDR memory[0xFE06]
const std::regex isNumber("X[+-]?[0-9A-F]+|#[+-]?[0-9]+");
const std::regex isValidSyntax("((ADD|AND) R[0-7],R[0-7],((R[0-7])|(X[+-]?[0-9A-F]+|#[+-]?[0-9]+)))|(BRN?Z?P? [^ ]+)|(JMP R[0-7])|(JSR [^ ]+)|(JSRR R[0-7])|((LD|LDI|LEA|ST|STI) R[0-7],[^ ]+)|((LDR|STR) R[0-7],R[0-7],(X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|(NOT R[0-7],R[0-7])|RET|RTI|(TRAP (X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|GETC|OUT|PUTS|IN|PUTSP|HALT|(\.ORIG (X[+-]?[0-9A-F]+|#[+-]?[0-9]+))|(\.FILL [^ ]+)|(\.BLKW [0-9]+)|(\.STRINGZ \".+\")|\.END");
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
    else{
        for(int i=0;i<num.length();i++){
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
std::string Assembler::numToStrBin(const std::string &num,int bits){
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
    if((bits==16&&decimalNum>(1<<bits)-1)||(bits!=16&&((negative&&decimalNum>(1<<bits)-1)||(!negative&&decimalNum>(1<<(bits-1))-1)))){
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
    bool flag=0;
    for(auto &line:code){
        if(line.substr(0,5)==".ORIG"){
            if(!flag)
                flag=1;
            else{
                std::cout<<"error:multiple .ORIG instructions";
                exit(1);
            }
        }
        else if(line==".END"){
            if(flag)
                flag=0;
            else{
                std::cout<<"error:multiple .END instructions";
                exit(1);
            }
        }
        else if(!flag)
            line="";
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
            binCode.push_back('!'+numToStrBin(word,16));
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
    if(num.length()!=3&&decimalNum>=(1<<(num.length()-1)))
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
Runner::Runner(){
    memset(memory,0,sizeof(memory));
    memset(reg,0,sizeof(reg));
    memset(PSR,0,sizeof(PSR));
    pc=0;
    SAVED_SSP=0x3000;
    SAVED_USP=0xFE00;
    DSR=1<<15;
    for(int i=0;i<=0x1F;i++)
        memory[i]=0x38C;//BAD_TRAP

    memory[0x20]=0x300;//TRAP_GETC
    memory[0x21]=0x306;//TRAP_OUT
    memory[0x22]=0x310;//TRAP_PUTS
    memory[0x23]=0x31F;//TRAP_IN
    memory[0x24]=0x340;//TRAP_PUTSP
    memory[0x25]=0x366;//TRAP_HALT

    for(int i=0x26;i<=0xFF;i++)
        memory[i]=0x38C;//BAD_TRAP

    memory[0x100]=0x3B3;//EX_PRIV
    memory[0x101]=0x3D6;//EX_ILL
    memory[0x102]=0x3F4;//EX_ACV

    for(int i=0x103;i<=0x1FF;i++)
        memory[i]=0x413;//BAD_INT

    //loader
    memory[0x200]=0x2C07;
    memory[0x201]=0x2007;
    memory[0x202]=0x1DBF;
    memory[0x203]=0x7180;
    memory[0x204]=0x2005;
    memory[0x205]=0x1DBF;
    memory[0x206]=0x7180;
    memory[0x207]=0x8000;
    memory[0x208]=0x3000;
    memory[0x209]=0x8002;
    memory[0x20A]=0x3000;

    //GETC
    memory[0x300]=0xA003;
    memory[0x301]=0x07FE;
    memory[0x302]=0xA002;
    memory[0x303]=0x8000;
    memory[0x304]=0xFE00;
    memory[0x305]=0xFE02;

    //OUT
    memory[0x306]=0x1DBF;
    memory[0x307]=0x7380;
    memory[0x308]=0xA205;
    memory[0x309]=0x07FE;
    memory[0x30a]=0xB004;
    memory[0x30b]=0x6380;
    memory[0x30c]=0x1DA1;
    memory[0x30d]=0x8000;
    memory[0x30e]=0xFE04;
    memory[0x30f]=0xFE06;

    //PUTS
    memory[0x310]=0x1DBF;
    memory[0x311]=0x7180;
    memory[0x312]=0x1DBF;
    memory[0x313]=0x7380;
    memory[0x314]=0x1220;
    memory[0x315]=0x6040;
    memory[0x316]=0x0403;
    memory[0x317]=0xF021;
    memory[0x318]=0x1261;
    memory[0x319]=0x0FFB;
    memory[0x31a]=0x6380;
    memory[0x31b]=0x1DA1;
    memory[0x31c]=0x6180;
    memory[0x31d]=0x1DA1;
    memory[0x31e]=0x8000;

    //IN
    memory[0x31f]=0xE00B;
    memory[0x320]=0xF022;
    memory[0x321]=0xF020;
    memory[0x322]=0xF021;
    memory[0x323]=0x1DBF;
    memory[0x324]=0x7180;
    memory[0x325]=0x5020;
    memory[0x326]=0x102A;
    memory[0x327]=0xF021;
    memory[0x328]=0x6180;
    memory[0x329]=0x1DA1;
    memory[0x32a]=0x8000;
    memory[0x32b]=0x000A;
    memory[0x32c]=0x0049;
    memory[0x32d]=0x006E;
    memory[0x32e]=0x0070;
    memory[0x32f]=0x0075;
    memory[0x330]=0x0074;
    memory[0x331]=0x0020;
    memory[0x332]=0x0061;
    memory[0x333]=0x0020;
    memory[0x334]=0x0063;
    memory[0x335]=0x0068;
    memory[0x336]=0x0061;
    memory[0x337]=0x0072;
    memory[0x338]=0x0061;
    memory[0x339]=0x0063;
    memory[0x33a]=0x0074;
    memory[0x33b]=0x0065;
    memory[0x33c]=0x0072;
    memory[0x33d]=0x003E;
    memory[0x33e]=0x0020;
    memory[0x33f]=0x0000;

    //PUTSP
    memory[0x340]=0x1DBF;
    memory[0x341]=0x7180;
    memory[0x342]=0x1DBF;
    memory[0x343]=0x7380;
    memory[0x344]=0x1DBF;
    memory[0x345]=0x7580;
    memory[0x346]=0x1DBF;
    memory[0x347]=0x7780;
    memory[0x348]=0x1220;
    memory[0x349]=0x6440;
    memory[0x34a]=0x201A;
    memory[0x34b]=0x5002;
    memory[0x34c]=0x040F;
    memory[0x34d]=0xF021;
    memory[0x34e]=0x5020;
    memory[0x34f]=0x1628;
    memory[0x350]=0x1000;
    memory[0x351]=0x14A0;
    memory[0x352]=0x0601;
    memory[0x353]=0x1021;
    memory[0x354]=0x1482;
    memory[0x355]=0x16FF;
    memory[0x356]=0x03F9;
    memory[0x357]=0x1020;
    memory[0x358]=0x0403;
    memory[0x359]=0xF021;
    memory[0x35a]=0x1261;
    memory[0x35b]=0x0FED;
    memory[0x35c]=0x6780;
    memory[0x35d]=0x1DA1;
    memory[0x35e]=0x6580;
    memory[0x35f]=0x1DA1;
    memory[0x360]=0x6380;
    memory[0x361]=0x1DA1;
    memory[0x362]=0x6180;
    memory[0x363]=0x1DA1;
    memory[0x364]=0x8000;
    memory[0x365]=0x00FF;

    //HALT
    memory[0x366]=0xE008;
    memory[0x367]=0xF022;
    memory[0x368]=0xA004;
    memory[0x369]=0x2204;
    memory[0x36a]=0x5001;
    memory[0x36b]=0xB001;
    memory[0x36c]=0x0FF9;
    memory[0x36d]=0xFFFE;
    memory[0x36e]=0x7FFF;
    memory[0x36f]=0x000A;
    memory[0x370]=0x000A;
    memory[0x371]=0x002D;
    memory[0x372]=0x002D;
    memory[0x373]=0x002D;
    memory[0x374]=0x0020;
    memory[0x375]=0x0048;
    memory[0x376]=0x0061;
    memory[0x377]=0x006C;
    memory[0x378]=0x0074;
    memory[0x379]=0x0069;
    memory[0x37a]=0x006E;
    memory[0x37b]=0x0067;
    memory[0x37c]=0x0020;
    memory[0x37d]=0x0074;
    memory[0x37e]=0x0068;
    memory[0x37f]=0x0065;
    memory[0x380]=0x0020;
    memory[0x381]=0x004C;
    memory[0x382]=0x0043;
    memory[0x383]=0x002D;
    memory[0x384]=0x0033;
    memory[0x385]=0x0020;
    memory[0x386]=0x002D;
    memory[0x387]=0x002D;
    memory[0x388]=0x002D;
    memory[0x389]=0x000A;
    memory[0x38a]=0x000A;
    memory[0x38b]=0x0000;

    //BAD_TRAP
    memory[0x38c]=0xE002;
    memory[0x38d]=0xF022;
    memory[0x38e]=0xF025;
    memory[0x38f]=0x000A;
    memory[0x390]=0x000A;
    memory[0x391]=0x002D;
    memory[0x392]=0x002D;
    memory[0x393]=0x002D;
    memory[0x394]=0x0020;
    memory[0x395]=0x0055;
    memory[0x396]=0x006E;
    memory[0x397]=0x0064;
    memory[0x398]=0x0065;
    memory[0x399]=0x0066;
    memory[0x39a]=0x0069;
    memory[0x39b]=0x006E;
    memory[0x39c]=0x0065;
    memory[0x39d]=0x0064;
    memory[0x39e]=0x0020;
    memory[0x39f]=0x0074;
    memory[0x3a0]=0x0072;
    memory[0x3a1]=0x0061;
    memory[0x3a2]=0x0070;
    memory[0x3a3]=0x0020;
    memory[0x3a4]=0x0065;
    memory[0x3a5]=0x0078;
    memory[0x3a6]=0x0065;
    memory[0x3a7]=0x0063;
    memory[0x3a8]=0x0075;
    memory[0x3a9]=0x0074;
    memory[0x3aa]=0x0065;
    memory[0x3ab]=0x0064;
    memory[0x3ac]=0x0020;
    memory[0x3ad]=0x002D;
    memory[0x3ae]=0x002D;
    memory[0x3af]=0x002D;
    memory[0x3b0]=0x000A;
    memory[0x3b1]=0x000A;
    memory[0x3b2]=0x0000;
    memory[0x3b3]=0xE002;
    memory[0x3b4]=0xF022;
    memory[0x3b5]=0xF025;
}
void Runner::load(std::vector<std::string>&code){
    for(auto &line:code){//load code into memory
        if(line[0]=='!')
            pc=binToNum(line.substr(1,16));
        else{
            memory[pc]=binToNum(line);
            pc++;
        }
    }
};
void Runner::run(std::vector<std::string>code){
    std::thread([this](){
        while(1){
            if(KBSR>=0){
                KBDR=getch();
                KBSR|=(1<<15);
            }
        }
    }).detach();
    std::thread([this](){
        while(1){
            if(DSR>=0){
                putchar(DDR);
                DSR|=(1<<15);
            }
        }
    }).detach();
    load(code);
    pc=0x200;//start at x200
    mode=0;
    while(true){
        if(KBSR&(1<<15)&&(KBSR&(1<<14))&&PL<4){
            KBSR&=0b0111111111111111;
            short numPSR=0;
            for(int i=0;i<16;i++){
                numPSR+=(PSR[i]<<i);
            }
            if(mode==1){
                SAVED_USP=reg[6];
                reg[6]=SAVED_SSP;
            }
            reg[6]--;
            memory[(unsigned)reg[6]]=numPSR;
            reg[6]--;
            memory[(unsigned)reg[6]]=pc;
            mode=0;
            PSR[10]=1;
            PSR[9]=0;
            PSR[8]=0;//以PL4执行中断处理程序
            pc=memory[0x180];
        }
        check(pc);
        std::string line=numToBin(memory[pc]);
        if(pc==0x303)
            KBSR&=0b0111111111111111;
        if(pc==0x30B)
            DSR&=0b0111111111111111;
        if(pc==0x36c)
            system("pause");
        pc++;
        std::string op=line.substr(0,4);
        short &dr=reg[binToNum(line.substr(4,3))];
        short &sr1=reg[binToNum(line.substr(7,3))];
        short &sr2=reg[binToNum(line.substr(13,3))];
        short &baser=reg[binToNum(line.substr(7,3))];
        short imm5=binToNum(line.substr(11,5));
        short offset6=binToNum(line.substr(10,6));
        short offset9=binToNum(line.substr(7,9));
        short offset11=binToNum(line.substr(5,11));
        short trapvect8=binToNum(line.substr(8,8));
        if(trapvect8<0)
            trapvect8+=256;
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
            reg[7]=pc;
            if(line[4]=='1')
                pc+=offset11;
            else
                pc+=baser;
        }
        else if(op=="0010"){//LD
            check(pc+offset9);
            dr=memory[pc+offset9];
            n=(dr<0);
            z=(dr==0);
            p=(dr>0);
        }
        else if(op=="1010"){//LDI
            check((unsigned short)memory[pc+offset9]);
            dr=memory[(unsigned short)memory[pc+offset9]];
            n=(dr<0);
            z=(dr==0);
            p=(dr>0);
        }
        else if(op=="0110"){//LDR
            check((unsigned short)baser+offset6);
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
            pc=reg[7];
        }
        else if(op=="0011"){//ST
            check(pc+offset9);
            memory[pc+offset9]=dr;
        }
        else if(op=="1011"){//STI
            check((unsigned short)memory[pc+offset9]);
            memory[(unsigned short)memory[pc+offset9]]=dr;
        }
        else if(op=="0111"){//STR
            check((unsigned short)baser+offset6);
            memory[(unsigned short)baser+offset6]=dr;
        }
        else if(op=="1111"){//TRAP
            short numPSR=0;
            for(int i=0;i<16;i++){
                numPSR+=(PSR[i]<<i);
            }
            if(mode==1){
                SAVED_USP=reg[6];
                reg[6]=SAVED_SSP;
            }
            reg[6]--;
            memory[(unsigned)reg[6]]=numPSR;
            reg[6]--;
            memory[(unsigned)reg[6]]=pc;
            mode=0;
            pc=memory[trapvect8];
        }
        else if(op=="1000"){//RTI
            pc=memory[(unsigned)reg[6]];
            reg[6]++;
            for(int i=0;i<16;i++){
                PSR[i]=memory[(unsigned)reg[6]]&(1<<i);
            }
            reg[6]++;
            if(mode==1){
                SAVED_SSP=reg[6];
                reg[6]=SAVED_USP;
            }
        }
    }
}
