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

const int INVALID_INPUT_PARAMETER = -1;
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
    bool immediate;
};

int fetchInstruction(control &controlInst, int32_t store[]) {
    //Check if a valid control instruction in control has been passed in
    if (controlInst.CI < 0 || controlInst.CI > MAX_MEM)
    {
        //Output error message and return out of the function
        std::cerr << "Invalid control CI input parameter" << std::endl;
        return INVALID_INPUT_PARAMETER;
    }


    controlInst.PI = store[controlInst.CI];
    return 0;
}

int printBits(int num) {
    //Check if a valid number has been passed in
    //2147483647 is the biggest number a 32 bit integer can hold
    if (num < -2147483648 || num > 2147483647)
    {
        //Output error message and return out of the function
        std::cerr << "Invalid number input parameter" << std::endl;
        return INVALID_INPUT_PARAMETER;
    }
    

    for (int i = 32 - 1; i >= 0; i--) {
        int bit = (num >> i) & 1;
        std::cout << bit;
    }
    std::cout << std::endl;

    return 0;
}

instruction decode(control controlInst) {
    //Check if a valid control instruction in control has been passed in
    if (controlInst.CI < 0 || controlInst.CI > MAX_MEM)
    {
        //Output error message and return out of the function
        std::cerr << "Invalid control CI input parameter" << std::endl;
        //return;
    }
    //Check if a valid present instruction in control has been passed in
    //2147483647 is the biggest number a 32 bit integer can hold
    if (controlInst.PI < -2147483648 || controlInst.PI > 2147483647)
    {
        //Output error message and return out of the function
        std::cerr << "Invalid control PI input parameter" << std::endl;
        //return;
    }

    int first_twelve = controlInst.PI & 0b111111111111;
    int last_sixteen = (controlInst.PI >> 16) &0b0111111111111111;
    int address = first_twelve | (last_sixteen << 12);
    return instruction {
        .operand = address,
        .opcode = (controlInst.PI >> 12) & 0b1111,
        .immediate = !!(controlInst.PI & (1 << 31))
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

int displayMemory(int32_t store[], int32_t numberOfLines, accumulator acc) {
    //store array length can't be checked because when array is passed it becomes a pointer
    //So instead of showing the size of the array it does the size of int32_t instead

    //Check if a valid number of Lines has been passed in
    //2147483647 is the biggest number a 32 bit integer can hold
    if (numberOfLines < -2147483648 || numberOfLines > MAX_MEM)
    {
        //Output error message and return out of the function
        std::cerr << "Invalid Number of Lines input parameter" << std::endl;
        return INVALID_INPUT_PARAMETER;
    }
    //Check if a valid accumulator has been passed in
    //2147483647 is the biggest number a 32 bit integer can hold
    if (acc < -2147483648 || acc > 2147483647)
    {
        //Output error message and return out of the function
        std::cerr << "Invalid accumulator input parameter" << std::endl;
        return INVALID_INPUT_PARAMETER;
    }

    std::cout << "Cum: " << acc << "Num: " << numberOfLines << std::endl;
    std::cout << "\033[2J\033[H";
    for (int32_t i = acc; i < numberOfLines; i++)
        displayLine(store[i]);

    return 0;
}

int execute(instruction inst, control &cont, accumulator &acc, int32_t store[]) {
    //Check if a valid operand in intruction has been passed in
    if (inst.operand < 0 || inst.operand > MAX_MEM)
    {
        //Output error message and return out of the function
        std::cout << inst.operand << std::endl;
        std::cerr << "Invalid intruction input parameter" << std::endl;
        return INVALID_INPUT_PARAMETER;
    }
    //Check if a valid control instruction in control has been passed in
    if (cont.CI < 0 || cont.CI > MAX_MEM)
    {
        //Output error message and return out of the function
        std::cerr << "Invalid control CI input parameter" << std::endl;
        return INVALID_INPUT_PARAMETER;
    }
    //Check if a valid present instruction in control has been passed in
    //2147483647 is the biggest number a 32 bit integer can hold
    if (cont.PI < -2147483648 || cont.PI > 2147483647)
    {
        //Output error message and return out of the function
        std::cerr << "Invalid control PI input parameter" << std::endl;
        return INVALID_INPUT_PARAMETER;
    }
    //Check if a valid accumulator has been passed in
    //2147483647 is the biggest number a 32 bit integer can hold
    if (acc < -2147483648 || acc > 2147483647)
    {
        //Output error message and return out of the function
        std::cerr << "Invalid accumulator input parameter" << std::endl;
        return INVALID_INPUT_PARAMETER;
    }

    //store array length can't be checked because when array is passed it becomes a pointer
    //So instead of showing the size of the array it does the size of int32_t instead

    int32_t value = store[inst.operand];
    switch (inst.opcode) {
        case 0b000:
            cont.CI = (value - 1);
            std::cout << cont.CI << std::endl;
            printBits(store[cont.CI+1]);
            break;
        case 0b001:
            cont.CI += (value - 1);
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
        case 0b1000:
            //msleep(inst.operand);
            break;
        case 0b1001:
            displayMemory(store,inst.operand,acc);
            break;
        default:
            std::cout << "Unknowen Instruction: " << inst.opcode << std::endl;
            halted=true;
            break;
    }
    return 0;
}

int readFile(int32_t *intPtr, std::string fileName) {
    //Check if the variable filename is empty
    if (fileName.empty())
    {
        //Output error message and return out of the function
        std::cerr << "Invalid fileName input parameter" << std::endl;
        return INVALID_INPUT_PARAMETER;
    }
    //Check if variable intPtr is a valid pointer
    if (intPtr == NULL)
    {
        //Output error message and return out of the function
        std::cerr << "Invalid pointer input parameter" << std::endl;
        return INVALID_INPUT_PARAMETER;
    }

    std::ifstream file(fileName);
    //Check if the file exists
    if (file.fail())
    {
        //Output error message and return out of the function
        std::cerr << "File does not exist." << std::endl;
        return INVALID_INPUT_PARAMETER;
    }

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
    return 0;
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
    std::string fileName = "machineCodeOut.txt";
    if (readFile(store, fileName) == INVALID_INPUT_PARAMETER)
    {
        //Ends the program if an invalid input was used
        return 0;
    }
    control cont;
    accumulator cum;
    instruction instruct;
    int count = 0;
    cont.CI = 25;
    while (!halted) {
        cont.CI++;
        if (fetchInstruction(cont, store) == INVALID_INPUT_PARAMETER)
        {
            //Ends the program if an invalid input was used
            return 0;
        }
        instruct = decode(cont);
        if (execute(instruct, cont, cum, store) == INVALID_INPUT_PARAMETER)
        {
            //Ends the program if an invalid input was used
            return 0;
        }
        
        
    }
    
    //displayMemory(store,32,0);
    return 0;
}