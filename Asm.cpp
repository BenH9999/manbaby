#include "Asm.hpp"

int main(){
    processAll();

    for (const std::string& line : asmFromFile) {
        std::cout << line << std::endl;
    }

    return 0;
}

void processAll(){
    readAsmFromFile();
    removeCommentsAndEmptyLines();
    mapAndRemoveLabels();
}

void removeCommentsAndEmptyLines() {
    for (std::string& s : asmFromFile) {
        size_t pos = s.find(';');
        if (pos != std::string::npos) {
            s = s.substr(0, pos);
        }
    }
    asmFromFile.erase(
        std::remove_if(asmFromFile.begin(), asmFromFile.end(),
            [](const std::string& line) {
                return line.find_first_not_of(" \r\t\n") == std::string::npos;
            }
        ),
        asmFromFile.end()
    );

}

void mapAndRemoveLabels(){
    int address = 0;
    for(std::string& s : asmFromFile){
        size_t pos = s.find(':');
        if(pos != std::string::npos){
            labelsFromAsm[s.substr(0,pos)] = address;
            //s.replace(0,pos+1,std::to_string(address)+":");
        }
        address++;
    }
    for(std::string& s : asmFromFile){
        for(std::pair<std::string,int> label : labelsFromAsm){
            size_t pos = s.find(label.first);
            //if(pos != )
        }
    }
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