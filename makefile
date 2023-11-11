all: asm sim test

asm: Asm.cpp
	g++ -o asm Asm.cpp -Wall

sim: Sim.cpp
	g++ -o sim Sim.cpp -Wall

test: flag_test.cpp
	g++ -o flag flag_test.cpp -Wall
