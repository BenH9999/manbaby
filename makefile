all: asm sim

asm: Asm.cpp
	g++ -O3 -o asm Asm.cpp -Wall

sim: Sim.cpp
	g++ -O3 -o sim Sim.cpp -Wall
