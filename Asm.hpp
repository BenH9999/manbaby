#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <regex>
#include <iomanip>
#include <bitset>

std::unordered_map<std::string, int> functionNumbers;
std::vector<std::string> asmFromFile;
std::vector<std::string> modifiedLines;
std::unordered_map<std::string, int> labelsFromAsm;
std::vector<std::string> labelNames;
std::vector<int> machineCode;

void processAll();
void mapFunctionNumbers();
void readAsmFromFile();
void removeCommentsAndEmptyLines();
void mapLabels();
void removeLabels();
void assemble();