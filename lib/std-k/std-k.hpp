#ifndef Kstd
#define Kstd

#include <iostream>
#include <array>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <sstream>

const std::string HOME = getenv("HOME");

void BreakPoint();
std::string ExecCmd(const std::string cmd, const int Output, int ExitStatus);
void WriteFileLines(std::vector<std::string> Lines, std::string File);
std::vector<std::string> ReadFileLines(std::string File);
void VPrint(std::vector<std::string> Input);
std::vector<std::string> SplitStr(std::string String, const char* Delim);
void Touch(std::string File);

#endif
