all: asm sim

asm: Asm.cpp
	g++ -o asm Asm.cpp -Wall

sim: Sim.cpp
	g++ -o sim Sim.cpp -Wall
