#include <iostream>
#include <string>
#include <cctype>

bool extended = false;
bool diffMem = false;
bool displayEveryCycle = true;
int32_t diffMemNum = -1;
std::string filename = "";

bool isAllDigits(std::string str) {
    for (char c : str)
    {
        if(!isdigit(c))
            return false;
    }
    return true;
}

void processArg(int& index, int argc, char* argv[]) {
    std::string arg = argv[index];
    std::string nextArg = "";
    if((argc - index - 1) >= 1)
        nextArg = argv[index+1];
    if(arg == "-e") {
        if(!extended) {
            extended = true;
        } else {
            std::cout << "Duplicated flag: " << arg << std::endl;
            exit(-1);
        }
    } else if (arg == "-m") {
        if (!diffMem) {
            index++;
            diffMem = true;

            if (nextArg.empty()) {
                std::cout << "Mem size not provided" << std::endl;
                exit(-1);
            } else {
                if (!isAllDigits(nextArg)) {
                    std::cout << "Mem size is not a number: " << nextArg << std::endl;
                    exit(-1);
                }

                diffMemNum = std::stoi(nextArg);
                if (diffMemNum > INT32_MAX || diffMemNum < 32) {
                    std::cout << "Mem size must be between 32 and " << INT32_MAX << std::endl;
                    exit(-1);
                }
            }
        } else {
            std::cout << "Duplicated flag: " << arg << std::endl;
            exit(-1);
        }
    } else if (arg == "-dd") {
        if(displayEveryCycle) {
            displayEveryCycle = false;
        } else {
            std::cout << "Duplicated flag: " << arg << std::endl;
            exit(-1);
        }
    } else if (filename.empty()) {
        filename = arg;
    } else {
        std::cout << "Unexpected flag: " << arg << std::endl;
        exit(-1);
    }
}

void processArgs(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++)
    {
        processArg(i,argc,argv);
    }
    if(filename.empty()){
        std::cout << "No file name provived" << std::endl;
        exit(-1);
    }
}


int main(int argc, char* argv[]) {
    processArgs(argc,argv);
    std::cout << "Extended instruction set: " << extended << std::endl;
    std::cout << "Mem size: " << (diffMem? diffMemNum : 32)  << std::endl;
    std::cout << "Display every cycle: " << displayEveryCycle << std::endl;
    std::cout << "Filename: " << filename << std::endl; 
    return 0;
}