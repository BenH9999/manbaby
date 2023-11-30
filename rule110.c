#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#define msleep(milliseconds) usleep(milliseconds*1000)

void printBits(int num) {
    for (int i = 32 - 1; i >= 0; i--) {
        int bit = (num >> i) & 1;
        printf("%d",bit);
    }
    printf("%c",'\n');
}

int rule[] = {
    [0b111] = 0, 
    [0b110] = 0, 
    [0b101] = 0, 
    [0b100] = 1,
    [0b011] = 1, 
    [0b010] = 1, 
    [0b001] = 1, 
    [0b000] = 0, 
};

void main() {
    int a = 0;
    int board = 0b00000000000000001000000000000000; // VAR 32768
    int backup = 0; // backup: VAR 0
    int i = 0;// i: VAR 0
    int less_i = 0;// less_i: VAR 0
    start:
        a = 0; // LDP #0
        printBits(board); // DIS #1
        msleep(1000); // WAT #1000
        a = board; // LDP board
        backup = a; // STO backup
        a = 0; // LDP #0;
        board = a; // STO board
        a = 32; // LDP #32;
        i = a; // STO i
        loop:
            a = i; // LDP i
            a = a - 1; // SUB #1
            less_i = a; // STO less_i
            a = backup; // LDP backup
            a = a >> (less_i); // BSR less_i
            a = a & 7; // AND #7
            a = rule[a]; // GET rule
            a = a << i; // BSL i
            a = a | board; // LOR board
            board = a; // STO board
            //board |= (rule[(backup >> (i - 1)) & 0b111] << i);
            a = less_i;
            i = a;
            a=i;
            if(!(a < 0))
                goto loop;
            else 
                goto start;
}

//int main() {
//    board = 0b00000000000000001000000000000000;
//    int a = 0;
//    int backup = 0;
//    int counter = 0;
//start:
//    a = 0; //LDP #0
//    printBits(board); // DIS 0
//    msleep(1000); // WAT #1000
//    a = board; //LDP board
//    backup = //STO backup
//    a = 0; // LDP #0
//    board = a; //STO board
//    a = 31; //LDP #31
//    counter = a; //STO counter
//    while (counter != 0)
//    {
//        board |= rule[backup & (0b111 << counter)];
//        counter--;
//    }
//    goto start;
//}