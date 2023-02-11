#include <iostream>
#include "silkcompile.h"
#include <filesystem>
#include <vector>
#include <bits/stdc++.h>

std::vector<unsigned char> parseDirectory(std::filesystem::path path, bool autoCompile, std::string dirName, unsigned short &startAddr);

int main(int argc, char *argv[]) {
    bool autoCompile = false;
    auto path = "fs";
    std::vector<unsigned char> result;
    std::string ofname = "output.bin";
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-silk")) {
            autoCompile = true;
        } else if (!strcmp(argv[i],"-o")) {
            ++i;
            ofname = argv[i];
        } else {
            path = argv[i];
        }
    }
    unsigned short startAddr = 0;
    result = parseDirectory(path, autoCompile, "/", startAddr);
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    result.push_back(255);
    result.push_back(255);
    result.push_back(0);
    result.push_back(0);
    std::ofstream output(ofname);
    output.write(reinterpret_cast<const char*>(result.data()), result.size());
    output.close();
    return 0;
}

std::vector<unsigned char> parseDirectory(std::filesystem::path path, bool autoCompile, std::string dirName, unsigned short &startAddr) {
    bool boot = false;
    if (dirName == "/" && (std::filesystem::exists(path/"boot") || (std::filesystem::exists(path/"boot.silk") && autoCompile))) boot = true;
    std::vector<unsigned char> result;
    //name header
    result.push_back(dirName.length()-1);
    result.push_back(dirName.at(0));
    //type header (assumes that directory contains <=16382 entries)
    result.push_back(0);
    result.push_back(1);
    //size header
    short size = 0;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().filename() == ".DS_Store") continue;
        ++size;
    }
    size = 4 * size + 5;
    result.push_back(size >> 8);
    result.push_back(size & 0xFF);
    //deal with address + name
    if (dirName.length() == 1) {
        result.push_back(startAddr >> 8);
        result.push_back(startAddr & 0xFF);
    } else {
        startAddr += dirName.length() + 3;
        result.push_back(startAddr >> 8);
        result.push_back(startAddr & 0xFF);

        for (char letter : dirName.substr(1)) {
            result.push_back(0);
            result.push_back(letter);
        }
        //repush header
        result.push_back(dirName.length()-1);
        result.push_back(dirName.at(0));
        result.push_back(0);
        result.push_back(1);
        result.push_back(size >> 8);
        result.push_back(size & 0xFF);
        result.push_back(startAddr >> 8);
        result.push_back(startAddr & 0xFF);
    }
    unsigned long int bootPB;
    if (boot) {
        bootPB = result.size();
    }
    startAddr += size;
    std::vector<std::vector<unsigned char>> files;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().filename() == ".DS_Store") continue;
        std::string name = entry.path().filename();
        std::vector<unsigned char> file;
        if (!entry.is_directory()) {
            if (autoCompile && name.length() >= 5 && name.substr(name.length()-5) == ".silk") {
                std::string compiled = silkCompile(entry.path());
                
                std::vector<unsigned char> fileContent;
                while (compiled != "") {
                    unsigned int num = std::stoi(compiled.substr(0, 5), nullptr, 16);
                    compiled.erase(0, 5);
                    fileContent.push_back((num >> 8) & 0xFF);
                    fileContent.push_back(num & 0xFF);
                }
                name = name.substr(0, name.length()-5);
                file.push_back(name.length()-1);
                file.push_back(name.at(0));
                file.push_back(0);
                file.push_back(0);
                file.push_back((fileContent.size()/2) >> 8);
                file.push_back((fileContent.size()/2) & 0xFF);
                startAddr += name.length() + 3;
                file.push_back(startAddr >> 8);
                file.push_back(startAddr & 0xFF);

                if (!(boot && (entry.path().filename() == "boot" || (entry.path().filename() == "boot.silk" && autoCompile)))) {
                    //also add file header to dir
                    result.push_back(name.length()-1);
                    result.push_back(name.at(0));
                    result.push_back(0);
                    result.push_back(0);
                    result.push_back((fileContent.size()/2) >> 8);
                    result.push_back((fileContent.size()/2) & 0xFF);
                    result.push_back(startAddr >> 8);
                    result.push_back(startAddr & 0xFF);
                } else {
                    result.insert(result.begin() + bootPB, file.begin(), file.end());
                }

                for (char letter : name.substr(1)) {
                    file.push_back(0);
                    file.push_back(letter);
                }

                startAddr += fileContent.size()/2;
                file.insert(file.end(), fileContent.begin(), fileContent.end());
            } else {
                startAddr += name.length() + 3;
                file.push_back(name.length()-1);
                file.push_back(name.at(0));
                file.push_back(0);
                file.push_back(0);
                file.push_back((entry.file_size()/2 + 1) >> 8);
                file.push_back((entry.file_size()/2 + 1) & 0xFF);
                file.push_back(startAddr >> 8);
                file.push_back(startAddr & 0xFF);

                if (!(boot && (entry.path().filename() == "boot" || (entry.path().filename() == "boot.silk" && autoCompile)))) {
                    result.push_back(name.length()-1);
                    result.push_back(name.at(0));
                    result.push_back(0);
                    result.push_back(0);
                    result.push_back((entry.file_size()/2 + 1) >> 8);
                    result.push_back((entry.file_size()/2 + 1) & 0xFF);
                    result.push_back(startAddr >> 8);
                    result.push_back(startAddr & 0xFF);
                } else {
                    result.insert(result.begin() + bootPB, file.begin(), file.end());
                }

                for (char letter : name.substr(1)) {
                    file.push_back(0);
                    file.push_back(letter);
                }

                std::ifstream f(entry.path());
                std::stringstream buffer;
                buffer << f.rdbuf();
                for (char c : buffer.str()) {
                    file.push_back(0);
                    file.push_back(c);
                }
                file.push_back(0);
                file.push_back(0);
            }
        } else {
            file = parseDirectory(entry.path(), autoCompile, name, startAddr);
            result.insert(result.end(), file.begin(), file.begin() + 8);
        }
        files.push_back(file);
    }
    result.push_back(0);
    result.push_back(0);
    for (auto file : files) {
        result.insert(result.end(), file.begin(), file.end());
    }
    return result;
}