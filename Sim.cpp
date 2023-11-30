#include <iostream>
#include <bitset>
#include <fstream>
#include <string>
#include <algorithm>
#include <unistd.h>
#define msleep(milliseconds) usleep(milliseconds*1000)

const int32_t MAX_MEM = 268435455;

bool halted = false;

typedef int32_t accumulator;

struct control {
    int32_t CI = 0;
    int32_t PI = 0;
};

struct instruction {
    int32_t operand;
    int32_t opcode;
    bool immediate;
};

struct optionsStruct
{
    std::string inputFile;
    bool extendedInstructions;
    bool extendedAddressing;
    bool alwaysDisplay;
    bool useDifferentMemorySize;
    int32_t MemorySize;
} options;


bool isAllDigits(std::string str) {
    for (char c : str)
    {
        if(!isdigit(c))
            return false;
    }
    return true;
}


int errorMessage(const char* msg, int32_t number) {
    std::cout << msg << number << std::endl;
    return -1;
}

int processArg(int& index, int argc, char* argv[]) {
    std::string arg = argv[index];
    std::string nextArg = "";
    if((argc - index - 1) >= 1)
        nextArg = argv[index+1];
    if(arg == "-e") {
        if(!options.extendedInstructions) {
            options.extendedInstructions = true;
            return 0;
        }
    } else if (arg == "-m") {
        if (!options.useDifferentMemorySize) {
            index++;
            options.useDifferentMemorySize = true;

            if (nextArg.empty()) {
                return errorMessage("Mem size not provided: ", 0);
            } else {
                if (!isAllDigits(nextArg)) {
                    std::cout << "Mem size is not a number: " << nextArg << std::endl;
                    return(-1);
                }

                options.MemorySize = std::stoi(nextArg);
                if (options.MemorySize > MAX_MEM || options.MemorySize < 32) {
                    std::cout << "Mem size must be between 32 and " << MAX_MEM << std::endl;
                    return(-1);
                }
                return 0;
            }
        }
    } else if (arg == "-d") {
        if(!options.alwaysDisplay) {
            options.alwaysDisplay = true;
            return 0;
        }
    } else if(arg == "-a") {
        if(!options.extendedAddressing) {
            options.extendedAddressing = true;
            return 0;
        }
    } else if (options.inputFile.empty()) {
        options.inputFile = arg;
        return 0;
    } else {
        std::cout << "Unexpected flag: " << arg << std::endl;
        return -1;
    }
    std::cout << "Duplicate flag: " << arg << std::endl;
    return -1;
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
    if(!options.useDifferentMemorySize)
        options.MemorySize = 32;
    return 0;
}

bool isValidMemoryAddress(int32_t address) {
    return address >= 0 && address < options.MemorySize;
}


int fetchInstruction(control &controlInst, int32_t store[]) {
    if(!isValidMemoryAddress(controlInst.CI))
        return errorMessage("[Fetch]: Invalid memory address: ",controlInst.CI);
    controlInst.PI = store[controlInst.CI];
    return 0;
}

void printBits(int num) {
    for (int i = 32 - 1; i >= 0; i--) {
        int bit = (num >> i) & 1;
        std::cout << bit;
    }
    std::cout << std::endl;
}

instruction decode(int32_t encodedInstruction) {
    int first_thirteen = encodedInstruction & 0b1111111111111;
    int last_fourteen = (encodedInstruction >> 17) &0b11111111111111;
    int address = first_thirteen | (last_fourteen << 13);
    return instruction {
        .operand = address,
        .opcode = (encodedInstruction >> 13) & 0b1111,
        .immediate = !!(encodedInstruction & (1 << 31))
    };
}

void printInstruction(instruction i) {
    std::cout << "Operand: " << i.operand << std::endl;
    std::cout << "Opcode: " << i.opcode << std::endl;
    std::cout << "Immediate: " << i.immediate << std::endl;
}

void displayLine(int32_t line) {
    for (int i = 0; i < 32; ++i) {
        if ((line >> i) & 1) {
            std::cout << "\033[47m ";
        } else {
            std::cout << "\033[40m ";
        }
    }
    std::cout << "\033[0m" << std::endl;
}

int displayMemory(int32_t store[], int32_t numberOfLines, accumulator acc) {
    std::cout << "\033[2J\033[H";
    for (int32_t i = acc; i < numberOfLines; i++) {
        if(!isValidMemoryAddress(i))
            return errorMessage("[DIS]: Invalid memory address: ",i);
        displayLine(store[i]);
    }
    return 0;
}

int execute(instruction inst, control &cont, accumulator &acc, int32_t store[]) {
    int32_t value = 0;
    if(inst.immediate)
        value = inst.operand;
    else {
        if(!isValidMemoryAddress(inst.operand)) {
            return errorMessage("[Execute]: Invalid memory address: ",inst.operand);
        }
        value = store[inst.operand];
    }
    if(inst.opcode >= 8 && !options.extendedInstructions) {
        std::cout << "Extended instruction set is not turned on" << std::endl;
        return errorMessage("[Execute]: Unknown Instruction: ",inst.opcode);
    }
    //std::cout << "CI: " << cont.CI << "Acc: " << acc << std::endl;
    switch (inst.opcode) {
        case 0b000:
            cont.CI = (value);
            break;
        case 0b001:
            cont.CI += (value);
            break;
        case 0b010:
            acc = -value;
            break;
        case 0b011:
            store[inst.operand] = acc;
            break;
        case 0b100:
        case 0b101:
            acc -= value;
            break;
        case 0b110:
            if (acc < 0)
               cont.CI++;
            break;
        case 0b111:
            halted = true;
            break;
        // Extened Instruction Set:
        case 0b1000: //wait
            msleep(value);
            break;
        case 0b1001: //display memory
            if(displayMemory(store,value,acc))
                return errorMessage("Display mem failed: ",0);
            break;
        case 0b1010: //Load positive
            acc = value;
            break;
        case 0b1011: //bit shift right
            acc = acc >> value;
            break;
        case 0b1100: //bit shift left
            acc = acc << value;
            break;
        case 0b1101: //logical and
            acc = acc & value;
            break;
        case 0b1110: //logical or
            acc = acc | value;
            break;
        case 0b1111: //get 
            acc = store[value + acc];
            break;
        default:
            return errorMessage("[Execute]: Unknown Instruction: ",inst.opcode);
            break;
    }
    
    return 0;
}

void readFile(int32_t *intPtr, std::string fileName) {
    std::ifstream file(fileName);
    std::string fileLine;
    int count = 0;
    while(getline(file, fileLine)){
        reverse(fileLine.begin(), fileLine.end());
        intPtr[count] = 0;
        for (long unsigned int i = 0; i < fileLine.length(); i++) {
            intPtr[count] |= ((fileLine[i]=='1') << ((fileLine.length() - i) - 1));
        }
        count++;
        if (count >= options.MemorySize) {
            std::cout<<"Error, file to long"<<std::endl;
            exit(-1);
        }
    }
    file.close();
}

int main(int argc, char *argv[]) {
    if(processArgs(argc, argv))
        return -1;

    std::cout << "inputFile: " << options.inputFile << std::endl;
    std::cout << "extendedInstructions: " << options.extendedInstructions << std::endl;
    std::cout << "extendedAddressing: " << options.extendedAddressing << std::endl;
    std::cout << "alwaysDisplay: " << options.alwaysDisplay << std::endl;
    std::cout << "useDifferentMemorySize: " << options.useDifferentMemorySize << std::endl;
    std::cout << "MemorySize: " << options.MemorySize << std::endl;

    int32_t* store = new int32_t[options.MemorySize];
    readFile(store, options.inputFile);
    control cont;
    accumulator cum;
    instruction instruct;
    cont.CI = 0;
    while (!halted) {
        cont.CI++;
        if(fetchInstruction(cont, store))
            return -1;
        instruct = decode(cont.PI);
        if(execute(instruct, cont, cum, store))
            return -1;
        if(options.alwaysDisplay) {
            displayMemory(store,10,0);
            msleep(500);
        }
    }
    return 0;
}