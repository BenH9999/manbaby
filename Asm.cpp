#include "Asm.hpp"

// Global variables and data structures
std::vector<std::string> labelNames;
std::unordered_map<std::string, int> functionNumbers;
std::vector<std::string> asmFromFile;
std::vector<std::string> modifiedLines;
std::unordered_map<std::string, int> symbolTable;
std::vector<int> machineCode;

// Structure to hold command-line options
struct optionsStruct
{
    std::string inputFile;
    std::string outputFile;
    bool extendedInstructions;
    bool extendedMemory;
    bool extendedAddressing;
} options;

// Function to print usage information
void printUsage() {
    std::cout << "Usage: ./asm [options] filename" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -e  Enable extended instructions" << std::endl;
    std::cout << "  -m  Enable extended memory" << std::endl;
    std::cout << "  -a  Enable extended addressing" << std::endl;
    std::cout << "  -o <output> Specify output file name" << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  ./asm -e -m -a -o output.bin input.asm" << std::endl;
}

// Function to process a single command-line argument
int processArg(int& index, int argc, char* argv[]) {
    std::string arg = argv[index];
    std::string nextArg = "";
    if((argc - index - 1) >= 1)
        nextArg = argv[index+1];

    if(arg == "-e" && !options.extendedInstructions) {
        options.extendedInstructions = true;
        return 0;
    } else if(arg == "-m" && !options.extendedMemory) {
        options.extendedMemory = true;
        return 0;
    } else if(arg == "-a" && !options.extendedAddressing) {
        options.extendedAddressing = true;
        return 0;
    } else if (arg == "-o" && options.outputFile.empty()) {
        index++;
        if (nextArg.empty()) {
            std::cout << "Output file not provided after '-o'" << std::endl;
            return -1;
        } else {
            options.outputFile = nextArg;
        }
        return 0;
    } else if (options.inputFile.empty()) {
        options.inputFile = arg;
        return 0;
    } else {
        std::cerr << "Unexpected flag: " << arg << std::endl;
        return -1;
    }
    std::cerr << "Duplicate flag: " << arg << std::endl; 
    return -1;
}

// Function to process all command-line arguments
int processArgs(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++)
    {
        if(processArg(i,argc,argv))
            return -1;
    }
    if(options.inputFile.empty()){
        std::cout << "No file name provided" << std::endl;
        return -1;
    }
    if(options.outputFile.empty())
        options.outputFile = "a.out";
    return 0;
}

// Main function
int main(int argc, char* argv[]){
    if(processArgs(argc, argv)) {
        printUsage();
        return -1;
    }
    const char* onOff[] = {"[off]", "[ON]"};

    std::cout << "Input File: " << options.inputFile << std::endl;
    std::cout << "Output File: " << options.outputFile << std::endl;
    std::cout << "Extended Instructions: " << onOff[options.extendedInstructions] << std::endl;
    std::cout << "Extended Memory: " << onOff[options.extendedMemory] << std::endl;
    std::cout << "Extended Addressing: " << onOff[options.extendedAddressing] << std::endl;
    return processAll();
}

// Sorting function for labels based on length
int length_sort(const std::string a, const std::string b) {
    return a.length() > b.length();
}

// Function to map labels to addresses
void mapLabels() {
    int address = 0;
    for(std::string& s : asmFromFile){
        size_t pos = s.find(':');
        if(pos != std::string::npos){
            std::string label = s.substr(0,pos);
            symbolTable[label] = address;
            labelNames.push_back(label) ;
            s = s.substr(pos+1);
        }
        address++;
    }
    std::sort(labelNames.begin(), labelNames.end(),length_sort);
}

// Function to process all steps of the assembler
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

// Function to check if a string can be parsed as an integer
bool tryParseInt(const std::string& toParse, int& buffer) {
    try {
        long long longBuffer = std::stoll(toParse);
        if(longBuffer > INT32_MAX || longBuffer < INT32_MIN) {
            std::cerr << "number " << longBuffer << " exceeds 32-bit limit!" << std::endl;
            return false;
        }
        buffer = (int)longBuffer;
        return true;
    } catch(const std::exception& e) {
        return false;
    }
    return true;
}

// Function to perform the assembly process
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
                std::cerr << "Failed to parse int: " << operandString << std::endl;
                std::cerr << "On line: [" << s << "]" << std::endl;
                return -1;
            }
            machineCode.push_back(buffer);
            continue;
        }

        if(functionNumbers.find(instructionString) == functionNumbers.end()) {
            std::cout << "Invalid instruction ["<<instructionString<<"]" << std::endl;
            if(!options.extendedInstructions)
                std::cerr << "Try turning on extended instruction set with -e" << std::endl;
            return -1;
        }

        int functionNumber = functionNumbers[instructionString];
        int operand;
        bool immediate = false;
        if(operandString[0] == '#') {
            if(!options.extendedAddressing) {
                std::cerr << "Extended addressing is not turned on, failed on line: " << s << std::endl;
                return -1;
            }
            immediate = true;
            operandString = operandString.substr(1,operandString.size()-1);
        }

        if(!tryParseInt(operandString, buffer)) {
            std::cerr << "Failed to parse as int: " << operandString << std::endl;
            std::cerr << "On line: [" << s << "]" << std::endl;
            return -1;
        }
        operand = buffer;
        int machineCodeInt = 0;

        if(operand >= 32 && !immediate && !options.extendedMemory) {
            std::cerr << "Warning execution will go out of bounds!" << std::endl;
        }

        if(operand >= 8192){
            int first = (operand & 0b1111111111111); 
            int second = (operand >> 13) & 0b11111111111111;
            machineCodeInt = first | (functionNumber << 13) | (second << 17) | ((immediate&0x1) << 31);
        } else {
            machineCodeInt = operand | (functionNumber << 13) | ((immediate&0x1) << 31);
        }

    machineCode.push_back(machineCodeInt);
    }
    return 0;
}

// Function to map function names to numbers
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
    functionNumbers["LDP"] = 10;
    functionNumbers["BSR"] = 11;
    functionNumbers["BSL"] = 12;
    functionNumbers["AND"] = 13;
    functionNumbers["LOR"] = 14;
    functionNumbers["GET"] = 15;
}

// Function to trim leading and trailing whitespaces from a string
void trim(std::string &s) {
    //Check if the line variable is empty
    if (s.empty())
        return;
    const char* ws = " \t\r\n";

    size_t start = s.find_first_not_of(ws);
    if (start != std::string::npos)
        s = s.substr(start);

    size_t end = s.find_last_not_of(ws);
    if (end != std::string::npos)
        s = s.substr(0,end+1);
}

// Function to replace labels with their corresponding addresses
void removeLabels() {
    for (std::string &line : asmFromFile)
    {
        for (std::string label : labelNames)
        {
            size_t location = line.find(label);
            if(location == std::string::npos)
                continue;
            line.replace(location, label.length(), std::to_string(symbolTable[label]));
        }
        trim(line);
    }   
}

// Function to remove comments and empty lines from assembly code
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

// Function to write machine code to a file
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
    std::cout << "Machine code written to " << options.outputFile << std::endl;
}

// Function to read assembly code from a file
void readAsmFromFile(){
    std::string fileName = options.inputFile;
    std::ifstream file(fileName);
    std::string buff;

    while(getline(file,buff)){
        asmFromFile.push_back(buff);
    }
    file.close();
}
