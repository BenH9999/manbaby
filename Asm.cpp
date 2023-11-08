#include "Asm.hpp"

int main(){
    readAsmFromFile();

    return 0;
}

void readAsmFromFile(){
    std::string fileName = "BabyTest1-Assembler.txt";
    std::ifstream file(fileName);
    std::string buff;

    while(getline(file,buff)){
        asmFromFile.push_back(buff);
    }
    file.close();
}