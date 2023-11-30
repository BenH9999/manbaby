#include "Asm.hpp"

std::vector<std::string> labelNames;

struct optionsStruct
{
    std::string inputFile;
    std::string outputFile;
    bool extendedInstructions;
    bool extendedMemory;
    bool extendedAddressing;
} options;


bool isAllDigits(std::string str) {
    for (char c : str)
    {
        if(!isdigit(c))
            return false;
    }
    return true;
}

int processArg(int& index, int argc, char* argv[]) {
    std::string arg = argv[index];
    std::string nextArg = "";
    if((argc - index - 1) >= 1)
        nextArg = argv[index+1];
    if(arg == "-e") {
        if(!options.extendedInstructions) {
            options.extendedInstructions = true;
        } else {
            std::cout << "Duplicated flag: " << arg << std::endl;
            return (-1);
        }
    } else if(arg == "-m") {
        if(!options.extendedMemory) {
            options.extendedMemory = true;
        } else {
            std::cout << "Duplicated flag: " << arg << std::endl;
            return (-1);
        }
    } else if(arg == "-a") {
        if(!options.extendedAddressing) {
            options.extendedAddressing = true;
        } else {
            std::cout << "Duplicated flag: " << arg << std::endl;
            return (-1);
        }
    } else if (arg == "-o") {
        if (options.outputFile.empty()) {
            index++;

            if (nextArg.empty()) {
                std::cout << "Output file not provided after '-o'" << std::endl;
                return (-1);
            } else {
                options.outputFile = nextArg;
            }
        } else {
            std::cout << "Duplicated flag: " << arg << std::endl;
            return (-1);
        }
    } else if (options.inputFile.empty()) {
        options.inputFile = arg;
    } else {
        std::cout << "Unexpected flag: " << arg << std::endl;
        return (-1);
    }
    return 0;
}

int processArgs(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++)
    {
        if(processArg(i,argc,argv))
            return -1;
    }
    if(options.inputFile.empty()){
        std::cout << "No file name provived" << std::endl;
        return -1;
    }
    if(options.outputFile.empty())
        options.outputFile = "a.out";
    return 0;
}


int main(int argc, char* argv[]){
    if(processArgs(argc, argv))
        return -1;

    const char* onOff[] = {"[off]", "[ON]"};

    std::cout << "Input File: " << options.inputFile << std::endl;
    std::cout << "Output File: " << options.outputFile << std::endl;
    std::cout << "Extended Instructions: " << onOff[options.extendedInstructions] << std::endl;
    std::cout << "Extended Memory: " << onOff[options.extendedMemory] << std::endl;
    std::cout << "Extended Addressing: " << onOff[options.extendedAddressing] << std::endl;
    return processAll();
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

int processAll(){
    //preprocessor
    readAsmFromFile();
    removeCommentsAndEmptyLines();
    mapLabels();
    removeLabels();

    //Main Assembler
    if(assemble())
        return -1;
    writeMachineCodeToFile();
    return 0;
}

bool tryParseInt(const std::string& toParse, int& buffer) {
    try {
        buffer = std::stoi(toParse);
    } catch(const std::exception& e) {
        return false;
    }
    return true;
}

int assemble(){
    mapFunctionNumbers();
    for(std::string s : asmFromFile){
        std::string instructionString;
        std::string operandString;
        int buffer = 0;
        size_t splitPosition = s.find(' ');

        if(splitPosition == std::string::npos) {
            instructionString = s;
            operandString = "0";
        } else {
            instructionString = s.substr(0,splitPosition);
            operandString = s.substr(splitPosition+1, s.size()-splitPosition);
        }

        if(instructionString == "VAR") {
            if(!tryParseInt(operandString, buffer)) {
                std::cout << "Failed to parse int: " << operandString << std::endl;
                return -1;
            }
            std::cout << buffer << std::endl;
            machineCode.push_back(buffer);
            continue;
        }

        if(functionNumbers.find(instructionString) == functionNumbers.end()) {
            std::cout << "Invalid instruction ["<<instructionString<<"]" << std::endl;
            if(!options.extendedInstructions)
                std::cout << "Try turning on extended instruction set with -e" << std::endl;
            return -1;
        }

        int functionNumber = functionNumbers[instructionString];
        int operand;
        bool immediate = false;
        if(operandString[0] == '#' && options.extendedAddressing) {
            immediate = true;
            operandString = operandString.substr(0,operandString.size()-1);
        }

        if(!tryParseInt(operandString, buffer)) {
            std::cout << "Failed to parse int: " << operandString << std::endl;
            return -1;
        }
        operand = buffer;
        int machineCodeInt = 0;

        if(operand >= 32 && !immediate && !options.extendedMemory) {
            std::cout << "Warning exectution will go out of bounds!" << std::endl;
        }

        if(operand >= 8192){
            int first = (operand & 0b1111111111111); 
            int second = (operand >> 13) & 16383;
            machineCodeInt = first | (functionNumber << 13) | (second << 16) | (immediate << 31);
        } else {
            machineCodeInt = operand | (functionNumber << 13) | (immediate << 31);
        }

    machineCode.push_back(machineCodeInt);
    }
    return 0;
}

void mapFunctionNumbers(){
    functionNumbers["JMP"] = 0;
    functionNumbers["JRP"] = 1;
    functionNumbers["LDN"] = 2;
    functionNumbers["STO"] = 3;
    functionNumbers["SUB"] = 4;
    functionNumbers["CMP"] = 6;
    functionNumbers["STP"] = 7;
    if(!options.extendedInstructions)
        return;
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
    std::ofstream file(options.outputFile);

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
    std::string fileName = options.inputFile;
    std::ifstream file(fileName);
    std::string buff;

    while(getline(file,buff)){
        asmFromFile.push_back(buff);
    }
    file.close();
}