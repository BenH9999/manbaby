 #include <iostream>
 #include <bitset>
using namespace std;

bool halted = false;

/*  STORE STRUCT

    Consists of 32 lines, 
    each line holding 32 bits.

    Bits are numbered from 0-31, 
    left to right. */

struct store{
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
0b00000000000000000000000000000000,};
};

struct accumulator{
    int32_t accumulator = 0;
};

/*
CONTROL STRUCT

both the control Instruction and Present instruction
values are initialised as 0, both chars will be updated
as the program proceeds
*/

struct control{
    int32_t CI = 0;
    int32_t PI = 0;
};

struct instruction{
    int32_t operand;
    int32_t opCode;
};

void increment(control &controlInst){
    controlInst.CI++;
}

void fetchInstruction(control &controlInst, store s){
    controlInst.PI = s.store[controlInst.CI];
}

instruction decode(control controlInst){

    instruction inst;
    inst.operand = controlInst.PI & 0b11111;
    inst.opCode = (controlInst.PI >> 13) & 0b111;

    return inst;
}

void execute(instruction inst, control &cont, accumulator &acc, store &s){
    if (inst.opCode == 0b000)
    {
        cont.CI = (s.store[inst.operand] - 1);
    }

    else if (inst.opCode == 0b001)
    {
        cont.CI += (s.store[inst.operand] - 1);
    }

    else if (inst.opCode == 0b010)
    {
        acc.accumulator = -s.store[inst.operand];
    }
    
    else if (inst.opCode == 0b011)
    {
        s.store[inst.operand] = acc.accumulator;
    }

    else if (inst.opCode == 0b100 || inst.opCode == 0b101)
    {
        acc.accumulator -= s.store[inst.operand];
    }
    
    else if (inst.opCode == 0b110)
    {
        if (acc.accumulator < 0)
        {
            increment(cont);
        }        
    }
    
    else if (inst.opCode == 0b111)
    {
        halted = true;
    }
}

int main(){

    store s;
    control cont;
    accumulator cum;

    instruction instruct;

    while (!halted)
    {
        increment(cont);
        fetchInstruction(cont, s);
        instruct = decode(cont);

        execute(instruct, cont, cum, s);
    }

    cout << s.store[9] << endl;

    return 0;
}