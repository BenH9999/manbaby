#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <regex>
#include <bitset>

std::unordered_map<std::string, int> functionNumbers;
std::vector<std::string> asmFromFile;
std::vector<std::string> modifiedLines;
std::unordered_map<std::string, int> labelsFromAsm;
std::vector<int> machineCode;

int processAll();
void mapFunctionNumbers();
void readAsmFromFile();
void removeCommentsAndEmptyLines();
void mapLabels();
void removeLabels();
int assemble();
void writeMachineCodeToFile();