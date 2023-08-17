#include <iostream>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>


int getOpReg(std::string &s);
std::string silkCompile(std::string ifname);

int linen;
/*
int main(int argc, char *argv[]) {
    std::string ifname, ofname;
    ifname = "input.silk";
    ofname = "output.bin";
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i],"-o")) {
            ++i;
            ofname = argv[i];
        } else {
            ifname = argv[i];
        }
    }

    std::ofstream output;

    output.open(ofname);
    std::string result = silkCompile(ifname);
    std::vector<unsigned char> data;
    while (result != "") {
        unsigned int num = std::stoi(result.substr(0, 5), nullptr, 16);
        result.erase(0, 5);
        data.push_back((num >> 8) & 0xFF);
        data.push_back(num & 0xFF);
    }
    output.write(reinterpret_cast<const char*>(data.data()), data.size());
    output.close();
    return 0;
}*/

std::string silkCompile(std::string ifname) {
    std::ifstream input;
    input.open(ifname);
    if (!input.is_open()) {
        std::cout << "invalid input file\n";
        exit(-1);
    }


    std::regex comments("\\/\\/.*");
    linen = -1;
    std::string result;
    while(!input.eof()) {
        std::string out;
        std::string s;
        ++linen;
        std::getline(input, s);
        if (s.length() == 0) continue;
        s = std::regex_replace(s, comments, "");
        bool whiteSpacesOnly = std::all_of(s.begin(),s.end(),isspace);
        if (whiteSpacesOnly) {
            continue;
        }
        s = std::regex_replace(s, std::regex("^ +| +$|( ) +"), "$1");
        if (s.at(0) == ' ') s.erase(0, 1);
        if (s.length() >= 3 && s.substr(0, 3) == "NOP") {
            out = "0000\n";
        } else if (s.length() >= 4 && s.substr(0, 3) == "IMM") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%01x001\n", getOpReg(s));

            if (s.length() > 0) {
                if (s.at(0) == '.') {
                    out += "!" + s + "\n";
                } else if (s.at(0) == '\'') {
                    char actual;
                    if (s.at(1) == '\\') {
                        if (s.at(2) == 'n') actual = '\n';
                        else if (s.at(2) == 'f') actual = '\f';
                        else if (s.at(2) == 't') actual = '\t';
                        else actual = s.at(2);
                    } else {
                        actual = s.at(1);
                    }
                    sprintf(out.data() + strlen(out.data()), "%04x\n", actual);
                } else if (s.substr(0, 2) == "0x") {
                    s.erase(0, 2);
                    sprintf(out.data() + strlen(out.data()), "%04x\n", stoi(s, nullptr, 16));
                } else {
                    sprintf(out.data() + strlen(out.data()), "%04x\n", stoi(s));
                }
            }
        } else if (s.length() >= 4 && s.substr(0, 3) == "ADD") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%3$01x%2$01x%1$01x2\n", getOpReg(s), getOpReg(s), getOpReg(s));
        } else if (s.length() >= 4 && s.substr(0, 3) == "SUB") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%3$01x%2$01x%1$01x3\n", getOpReg(s), getOpReg(s), getOpReg(s));
        } else if (s.length() >= 4 && s.substr(0, 3) == "AND") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%3$01x%2$01x%1$01x4\n", getOpReg(s), getOpReg(s), getOpReg(s));
        } else if (s.length() >= 4 && s.substr(0, 3) == "NOR") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%3$01x%2$01x%1$01x5\n", getOpReg(s), getOpReg(s), getOpReg(s));
        } else if (s.length() >= 4 && s.substr(0, 3) == "BSL") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%3$01x%2$01x%1$01x6\n", getOpReg(s), getOpReg(s), getOpReg(s));
        } else if (s.length() >= 4 && s.substr(0, 3) == "BSR") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%3$01x%2$01x%1$01x7\n", getOpReg(s), getOpReg(s), getOpReg(s));
        } else if (s.length() >= 4 && s.substr(0, 3) == "BGE") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%3$01x%2$01x%1$01x8\n", getOpReg(s), getOpReg(s), getOpReg(s));
        } else if (s.length() >= 4 && s.substr(0, 3) == "BRE") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%3$01x%2$01x%1$01x9\n", getOpReg(s), getOpReg(s), getOpReg(s));
        } else if (s.length() >= 4 && s.substr(0, 3) == "LOD") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%2$01x%1$01x0a\n", getOpReg(s), getOpReg(s));
        } else if (s.length() >= 4 && s.substr(0, 3) == "STR") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%2$01x%1$01x0b\n", getOpReg(s), getOpReg(s));
        } else if (s.length() >= 4 && s.substr(0, 3) == "PSH") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%01x00c\n", getOpReg(s));
        } else if (s.length() >= 4 && s.substr(0, 3) == "POP") {
            s.erase(0, 4);
            sprintf(out.data() + strlen(out.data()), "%01x00d\n", getOpReg(s));
        } else if (s.length() >= 5 && s.substr(0, 4) == "BIMM") {
            s.erase(0, 5);
            sprintf(out.data() + strlen(out.data()), "%01x00e\n", getOpReg(s));
            while (s.length() > 0) {
                int white = s.find(" ");
                if (white == -1) white = s.length()-1;
                if (s.at(0) == ' ') {
                    s.erase(0, 1);
                } else if (s.at(0) == '.') {
                    sprintf(out.data() + strlen(out.data()), "!%s\n", s.substr(0, white+1).data());
                    s.erase(0, white + 1);
                } else if (s.substr(0, 2) == "0x") {
                    s.erase(0, 2);
                    white = s.find(" ");
                    if (white == -1) white = s.length()-1;
                    sprintf(out.data() + strlen(out.data()), "%04d\n", stoi(s.substr(0, white+1), nullptr, 16));
                    s.erase(0, white + 1);
                } else if (s.at(0) == '\"') {
                    s.erase(0, 1);
                    while (s.length() > 0 && s.at(0) != '\"') {
                        if (s.at(0) == '\\') {
                            char actual = 0;
                            if (s.at(1) == '\\') actual = '\\';
                            if (s.at(1) == 'n') actual = '\n';
                            if (s.at(1) == 'f') actual = '\f';
                            if (s.at(1) == 't') actual = '\t';
                            if (s.at(1) == '\"') actual = '\"';
                            if (s.at(1) == '\'') actual = '\'';
                            if (s.at(1) == '/') actual = '/';
                            sprintf(out.data() + strlen(out.data()), "%04x\n", actual);
                            s.erase(0, 1);
                            result += out.data();
                            out = "";
                        } else {
                            sprintf(out.data() + strlen(out.data()), "%04x\n", s.at(0));
                            result += out.data();
                            out = "";
                        }
                        s = s.substr(1);
                    }
                    s.erase(0, 1);
                } else if (s.at(0) == '[' || s.at(0) == ']') {
                    s.erase(0, 1);
                } else {
                    sprintf(out.data() + strlen(out.data()), "%04x\n", stoi(s.substr(0, white+1)));
                    s.erase(0, white + 1);
                }
            }
        } else if (s.length() >= 5 && s.substr(0, 4) == "SCAL") {
            s.erase(0, 5);
            std::string syscall = s.substr(0, s.find(" "));
            int op1 = 0;
                 if (syscall == "EXIT") op1 = 0;
            else if (syscall == "CREAT") op1 = 1;
            else if (syscall == "OPEN") op1 = 2;
            else if (syscall == "CLOSE") op1 = 3;
            else if (syscall == "READ") op1 = 4;
            else if (syscall == "WRITE") op1 = 5;
            else if (syscall == "LSEEK") op1 = 6;
            else if (syscall == "LINK") op1 = 7;
            else if (syscall == "UNLINK") op1 = 8;
            else if (syscall == "STAT") op1 = 9;
            else if (syscall == "EXEC") op1 = 10;
            else if (syscall == "FORK") op1 = 11;
            else if (syscall == "GETPID") op1 = 12;
            else if (syscall == "WAITPID") op1 = 13;
            else if (syscall == "KILL") op1 = 14;
            else if (syscall == "CHDIR") op1 = 15;
            else if (syscall == "STIME") op1 = 16;
            else if (syscall == "TIME") op1 = 17;
            else if (syscall == "RDIR") op1 = 18;
            else if (syscall == "MALLOC") op1 = 19;
            else if (syscall == "FREE") op1 = 20;
            else if (syscall == "REALLOC") op1 = 21;
            else if (syscall == "PUTS") op1 = 22;
            else if (syscall == "PUTC") op1 = 23;
            else if (syscall == "PUTCI") op1 = 24;
            else if (syscall == "GETS") op1 = 25;
            else if (syscall == "GETC") op1 = 26;
            else if (syscall == "MEMCMP") op1 = 27;
            else if (syscall == "MEMCPY") op1 = 28;
            else if (syscall == "RAND") op1 = 29;
            else if (syscall == "STRCMP") op1 = 30;
            else if (syscall == "STRCPY") op1 = 31;
            else if (syscall == "STRLEN") op1 = 32;
            else if (syscall == "LODOSVAR") op1 = 33;
            else if (syscall == "STROSVAR") op1 = 34;
            else if (syscall == "GETDIRNAME") op1 = 35;
            else if (syscall == "NTHSTRING") op1 = 36;
            else if (syscall == "FLEN") op1 = 37;
            else printf("Invalid Syscall on line: %d", linen);
            s.erase(0, s.find(" ") + 1);
            int op2;
            if (op1 == 24) {
                sprintf(out.data() + strlen(out.data()), "0%02xf\n", op1);
                if (s.length() > 0) {
                    if (s.at(0) == '.') {
                        sprintf(out.data() + strlen(out.data()), "!%s\n", s.data());
                    } else if (s.at(0) == '\'') {
                        char actual;
                        if (s.at(1) == '\\') {
                            if (s.at(2) == 'n') actual = '\n';
                            else if (s.at(2) == 'f') actual = '\f';
                            else if (s.at(2) == 't') actual = '\t';
                            else actual = s.at(2);
                        } else {
                            actual = s.at(1);
                        }
                        sprintf(out.data() + strlen(out.data()), "%04x\n", actual);
                    } else if (s.substr(0, 2) == "0x") {
                        s.erase(0, 2);
                        sprintf(out.data() + strlen(out.data()), "%04d\n", stoi(s, nullptr, 16));
                    } else {
                        sprintf(out.data() + strlen(out.data()), "%04x\n", stoi(s));
                    }
                }
            } else {
                if (s == "CWD") op2 = 0;
                else if (s == "ROOT") op2 = 1;
                else if (s == "PATH") op2 = 2;
                else if (s == "SHELL") op2 = 3;
                else if (s == "ECHO") op2 = 4;
                else if (s == "MEMLIMIT") op2 = 5;
                else if (s == "BITS") op2 = 6;
                else if (s == "MEM0") op2 = 7;
                else if (s == "MALLOCSADDR") op2 = 8;
                else if (s == "USERNAME") op2 = 9;
                else if (s == "SEEKOFFSET") op2 = 10;
                else if (s == "CPP") op2 = 11;
                else if (s == "PROCNAME") op2 = 12;
                else if (s == "EXIT") op2 = 13;
                else if (s == "STARTUPPROG") op2 = 14;
                else if (s == "ERROR") op2 = 15;
                else op2 = getOpReg(s);
                sprintf(out.data() + strlen(out.data()), "%01x%02xf\n", op2, op1);
            }
        } else {
            s += '\n';
            strcpy(out.data(), s.data());
        }

        result += out.data();
    }
    //get labels
    std::vector<int> labelAddrs;
    std::vector<std::string> labels;
    std::string temp = result;
    if (temp.at(temp.length()-1) != '\n') temp += '\n';
    for (linen = 0; temp != ""; ++linen) {
        unsigned long int endLine = temp.length()-1;
        if (temp.find('\n') != (unsigned long int)(-1)) endLine = temp.find('\n');
        std::string line = temp.substr(0, endLine);
        if (line.at(0) == '.') {
            labelAddrs.push_back(linen);
            labels.push_back(line);
            --linen;
        }
        temp.erase(0, endLine + 1);
    }
    
    
    temp = result;
    result = "";
    while (temp != "") {
        unsigned long int endLine = temp.length()-1;
        if (temp.find('\n') != (unsigned long int)(-1)) endLine = temp.find('\n');
        std::string line = temp.substr(0, endLine);
        if (line.at(0) == '.') {
            temp.erase(0, endLine + 1);
            continue;
        } else if (line.at(0) == '!') {
            line.erase(0, 1);
            for (unsigned long int i = 0 ;; ++i) {
                if (labels.at(i) == line) {
                    sprintf(line.data(), "%04x", labelAddrs[i]);
                    break;
                } else if (i == labels.size() - 1) {
                    std::cout << "Invalid label: " << line.data() << '\n';
                    break;
                }
            }
        }
        result += line.data();
        result += " ";
        temp.erase(0, endLine + 1);
    }
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    input.close();
    return result;
    
}

int getOpReg(std::string &s) {
    int op;
    if (s.length() < 2) printf("Invalid Operand on line %d", linen);
    if (s.length() > 2 && isdigit(s.at(2))) {
        op = stoi(s.substr(1, 2));
        s.erase(0, 4);
    } else {
        op = stoi(s.substr(1, 1));
        s.erase(0, 3);
    }
    return op;
}
