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

//FUNCTION WITH BROKEN REPLACEMENT
/*void mapAndRemoveLabels(){
    int address = 0;
    for(std::string& s : asmFromFile){
        size_t pos = s.find(':');
        if(pos != std::string::npos){
            labelsFromAsm[s.substr(0,pos)] = address;
        }
        address++;
    }
    for(std::string& s : asmFromFile){
        for(std::pair<std::string,int> label : labelsFromAsm){
            //std::regex labelRegex("\\b" + label.first + "\\b");
            //s = std::regex_replace(s, labelRegex, std::to_string(label.second));
            std::string labelWithSpace = label.first + " ";
            std::string replacement = std::to_string(label.second) + " ";

            size_t pos = 0;

            // Replace all occurrences of the label in the line
            while ((pos = s.find(labelWithSpace, pos)) != std::string::npos) {
                s.replace(pos, labelWithSpace.length(), replacement);
                pos += replacement.length();
            }
        }
    }
}*/

//FUNCTION WITH BROKEN SPACING
void mapAndRemoveLabels() {
    int address = 0;

    for (const std::string& s : asmFromFile) {
        size_t pos = s.find(':');
        if (pos != std::string::npos) {
            labelsFromAsm[s.substr(0, pos)] = address;
        }
        address++;
    }

    for (std::string& s : asmFromFile) {
        for (const auto& label : labelsFromAsm) {
            std::string labelWithSpace = label.first;
            std::string replacement = std::to_string(label.second);

            std::regex labelRegex("\\b" + label.first + "\\b");
            s = std::regex_replace(s, labelRegex, replacement);
        }

        s = std::regex_replace(s, std::regex("\\s+"), " ");
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