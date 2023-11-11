#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>

std::vector<std::string> asmFromFile;
std::unordered_map<std::string, int> labelsFromAsm;

void processAll();
void readAsmFromFile();
void removeCommentsAndEmptyLines();
void mapAndRemoveLabels();