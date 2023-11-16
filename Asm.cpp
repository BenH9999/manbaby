#include "Asm.hpp"

std::vector<std::string> labelNames;

int main(){
    processAll();

    for (const std::string& line : asmFromFile) {
        std::cout << line << std::endl;
    }
    std::cout << std::endl;
    for(size_t i = 0; i < machineCode.size();i++){
        for (int j = 0; j < 32; j++)
        {
            std::cout << (( machineCode[i] >> j ) & 0x1);
        }
        std::cout << std::endl;
    }
    
    return 0;
}

int length_sort(const std::string a, const std::string b) {
    return a.length() > b.length();
}

void mapLabels() {
    int address = 0;
    for(std::string& s : asmFromFile){
        size_t pos = s.find(':');
        if(pos != std::string::npos){
            std::string label = s.substr(0,pos);
            labelsFromAsm[label] = address;
            labelNames.push_back(label) ;
            s = s.substr(pos+1);
        }
        address++;
    }
    std::sort(labelNames.begin(), labelNames.end(),length_sort);
}

void processAll(){
    //preprocessor
    readAsmFromFile();
    removeCommentsAndEmptyLines();
    mapLabels();
    removeLabels();

    //Main Assembler
    assemble();
    writeMachineCodeToFile();
}

void assemble(){
    mapFunctionNumbers();
    for(std::string s : asmFromFile){
        std::istringstream iss(s);
        std::string instruction;
        iss >> instruction;

        if(instruction != "VAR"){
            int operand, functionNo, machineCodeInt;

            functionNo = functionNumbers[instruction];
            iss >> operand;

            if(operand > 4095){
                int first = (operand & 0b111111111111); 
                int second = operand >> 12;
                machineCodeInt = first | (functionNo << 12) | (second << 16);
            }   
            else  {
                machineCodeInt = operand | (functionNo << 12);
            }

            machineCode.push_back(machineCodeInt);
        }
        else{
            int varValue;
            iss >> varValue;
            machineCode.push_back(varValue);
        }
    }
}

void mapFunctionNumbers(){
    functionNumbers["JMP"] = 0;
    functionNumbers["JRP"] = 1;
    functionNumbers["LDN"] = 2;
    functionNumbers["STO"] = 3;
    functionNumbers["SUB"] = 4;
    functionNumbers["CMP"] = 6;
    functionNumbers["STP"] = 7;
    functionNumbers["WAT"] = 8;
    functionNumbers["DIS"] = 9;
}

void trim(std::string &s) {
    const char* ws = " \t\r\n";

    size_t start = s.find_first_not_of(ws);
    if (start != std::string::npos)
        s = s.substr(start);

    size_t end = s.find_last_not_of(ws);
    if (end != std::string::npos)
        s = s.substr(0,end+1);
}

void removeLabels() {
    for (std::string &line : asmFromFile)
    {
        for (std::string label : labelNames)
        {
            size_t location = line.find(label);
            if(location == std::string::npos)
                continue;
            line.replace(location, label.length(), std::to_string(labelsFromAsm[label]));
        }
        trim(line);
    }   
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

void writeMachineCodeToFile(){
    std::ofstream file("machineCodeOut.txt");

    for(size_t i = 0; i < machineCode.size(); i++){
        for (int j = 0; j < 32; j++)
        {
            file << (( machineCode[i] >> j ) & 0x1);
        }
        file << "\n";
    }

    file.close();
}

void readAsmFromFile(){
    std::string fileName = "programs/src/bad_apple.asm";
    std::ifstream file(fileName);
    std::string buff;

    while(getline(file,buff)){
        asmFromFile.push_back(buff);
    }
    file.close();
}