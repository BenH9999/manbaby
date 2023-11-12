#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <regex>
#include <iomanip>

std::vector<std::string> asmFromFile;
std::vector<std::string> modifiedLines;
std::unordered_map<std::string, int> labelsFromAsm;

void processAll();
void readAsmFromFile();
void removeCommentsAndEmptyLines();
void mapAndRemoveLabels();