#include <iostream>
#include <bitset>

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
    return (instruction) {
        .operand = controlInst.PI & 0b11111,
        .opcode = (controlInst.PI >> 13) & 0b111
    };
}

void execute(instruction inst, control &cont, accumulator &acc, int32_t store[]) {
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
        default:
            break;
    }
}

int main() {
    int32_t store[32] = { 
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
    };
    control cont;
    accumulator cum;
    instruction instruct;

    while (!halted) {
        cont.CI++;
        fetchInstruction(cont, store);
        instruct = decode(cont);
        execute(instruct, cont, cum, store);
    }

    std::cout << store[9] << std::endl;
    return 0;
}