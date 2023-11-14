#include <iostream>
#include <bitset>
#include <fstream>
#include <string>
#include <algorithm>
#ifdef _WIN32
    #include <windows.h> // Unsure if works.
    typedef signed int int32_t;
#else
    #include <unistd.h>
#endif
#define msleep(milliseconds) usleep(milliseconds*1000)

const int32_t MAX_MEM = 268435455;
int32_t mem_size = 32;

bool halted = false;

typedef int32_t accumulator;

struct control {
    int32_t CI = 0;
    int32_t PI = 0;
};

struct instruction {
    int32_t operand;
    int32_t opcode;
};

void fetchInstruction(control &controlInst, int32_t store[]) {
    controlInst.PI = store[controlInst.CI];
}

instruction decode(control controlInst) {
    int first_twelve = controlInst.PI & 0b111111111111;
    int last_sixteen = controlInst.PI >> 16;
    int address = first_twelve | (last_sixteen << 12);

    return (instruction) {
        .operand = address,
        .opcode = (controlInst.PI >> 12) & 0b1111
    };
}

void displayLine(int32_t line) {
    for (int i = 0; i < 32; ++i) {
        if ((line >> i) & 1) {
            std::cout << "\033[47m ";
        } else {
            std::cout << "\033[40m "; // Unicode character for a white square
        }
    }
    std::cout << "\033[0m" << std::endl;
}

void displayMemory(int32_t store[], int32_t numberOfLines, accumulator acc) {
    std::cout << "\033[2J\033[H";
    for (int32_t i = acc; i < numberOfLines; i++)
        displayLine(store[i]);
}

void execute(instruction inst, control &cont, accumulator &acc, int32_t store[]) {
    //std::cout << "CI: " << cont.CI << std::endl;
    //if (cont.CI == 26) {
    //    //displayLine(store[26]);
    //    //std::cout << inst.opcode << std::endl;
    //}

    switch (inst.opcode) {
        case 0b000:
            cont.CI = (store[inst.operand] - 1);
            break;
        case 0b001:
            cont.CI += (store[inst.operand] - 1);
            break;
        case 0b010:
            acc = -store[inst.operand];
            break;
        case 0b011:
            store[inst.operand] = acc;
            break;
        case 0b100:
        case 0b101:
            acc -= store[inst.operand];
            break;
        case 0b110:
            if (acc < 0)
               cont.CI++;
            break;
        case 0b111:
            halted = true;
            break;
        // Extened Instruction Set:
        case 0b1000:
            msleep(inst.operand);
            break;
        case 0b1001:
            displayMemory(store,inst.operand,acc);
            break;
        default:
            std::cout << "Unknowen Instruction: " << inst.opcode << std::endl;
            halted=true;
            break;
    }
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
        if (count >= MAX_MEM) {
            std::cout<<"Error, file to long";
            exit(-1);
        }
    }
    file.close();
}

int main() {
    /*int32_t store[32] = { 
        0b00000000000000000000000000000000,
        0b00000000000000000100000000000111,
        0b00000000000000001000000000001000,
        0b00000000000000000110000000001001,
        0b00000000000000000100000000001001,
        0b00000000000000000110000000001001,
        0b00000000000000001110000000000000,
        0b00000000000000000000010000000001,
        0b00000000000000000000001001101101,
        0b00000000000000000000000000000000,
    }*/;

    int32_t* store = new int32_t[MAX_MEM];
    std::string fileName = "programs/bad_apple_works.mc";
    readFile(store, fileName);
    control cont;
    accumulator cum;
    instruction instruct;
    while (!halted) {
        cont.CI++;
        fetchInstruction(cont, store);
        instruct = decode(cont);
        execute(instruct, cont, cum, store);
    }

    //std::cout << store[18] << std::endl;
    return 0;
}