#include "Asm.hpp"

int main(){
    processAll();

    for (const std::string& line : asmFromFile) {
        std::cout << line << std::endl;
    }
    std::cout << "\n";
    for(size_t i = 0; i < machineCode.size();i++){
        for (int j = 0; j < 32; j++)
        {
            std::cout << (( machineCode[i] >> j ) & 0x1);
        }
        std::cout << std::endl;
    }

    return 0;
}

void mapLabels() {
    int address = 0;
    for(std::string& s : asmFromFile){
        size_t pos = s.find(':');
        if(pos != std::string::npos){
            labelsFromAsm[s.substr(0,pos)] = address;
            labelNames.push_back(s.substr(0,pos));
            s = s.substr(pos+1);
        }
        address++;
    }
    std::sort(labelNames.begin(), labelNames.end(), [](const std::string a, const std::string b) {
        return a.length() > b.length();
    });
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
            int operand, functionNo;

            functionNo = functionNumbers[instruction];

            iss >> operand;

            int machineCodeInt = (functionNo << 13);
            if(operand > 4095){
                int first = (operand & 0b111111111111); 
                int second = operand & 0b0000000000001111111111111111;
                machineCodeInt = first | functionNo << 13 | second << 16;
            }   
            else  {
                machineCodeInt |=  operand;
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
            std::string pattern = "\\b" + label + "\\b";
            line = std::regex_replace(line, std::regex(pattern), std::to_string(labelsFromAsm[label]));
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
    std::string fileName = "BabyTest1-Assembler.txt";
    std::ifstream file(fileName);
    std::string buff;

    while(getline(file,buff)){
        asmFromFile.push_back(buff);
    }
    file.close();
}