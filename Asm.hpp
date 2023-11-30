#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <bitset>

int processAll();
void mapFunctionNumbers();
void readAsmFromFile();
void removeCommentsAndEmptyLines();
void mapLabels();
void removeLabels();
int assemble();
void writeMachineCodeToFile();