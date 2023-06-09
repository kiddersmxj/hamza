#include "std-k.hpp"
#include <string>

// NULL function to attach breakpoint to in gdb
void BreakPoint(void) {
    std::cout << "";
}

std::string ExecCmd(const std::string cmd, const int Output, int ExitStatus) {
	auto pPipe = ::popen(cmd.c_str(), "r");
	if(pPipe == nullptr)
	{
	    throw std::runtime_error("Cannot open pipe");	
	}

    std::array<char, 256> buffer;

    std::string result;

	while(not feof(pPipe)) {
	    auto bytes = fread(buffer.data(), 1, buffer.size(), pPipe);
	    result.append(buffer.data(), bytes);
	}

	auto rc = ::pclose(pPipe);

    if(Output == 1)
        std::cout << result << std::endl;

    if(WIFEXITED(rc)) {
	    ExitStatus = WEXITSTATUS(rc);
	}

	return result;
}

void VPrint(std::vector<std::string> Input) {
	for(std::string Output: Input)
	    std::cout << Output << std::endl;
}

void WriteFileLines(std::vector<std::string> Lines, std::string File) {
    std::fstream FileToWrite(File, std::ios::out);
	if (!FileToWrite) {
        std::cout << "File not created!" << std::endl;
	}
	else {
        for(std::string Line: Lines)
            FileToWrite << Line << std::endl;
		FileToWrite.close();
	}
}

std::vector<std::string> ReadFileLines(std::string File) {
    std::string Line;
    std::vector<std::string> Output;
    std::ifstream FileToRead(File);

    while(getline(FileToRead, Line)) {
        Output.push_back(Line);
    }

    FileToRead.close();
    return Output;
}

void Touch(std::string File) {
    std::fstream fs;
    fs.open(File, std::ios::out);
    fs.close();
}

int GetIndex(std::vector<std::string> v, std::string K) {
    auto it = std::find(v.begin(), v.end(), K);
  
    // If element was found
    if (it != v.end()) {
        // calculating the index of K
        int index = it - v.begin();
        return index;
    } else {
        // If the element is not present in the vector
        return -1;
    }
}

std::string StripTrailingNL(std::string Input) {
    Input.erase(std::remove(Input.begin(), Input.end(), '\n'), Input.cend());
    return Input;
}

void SplitString(std::string &str, char delim, std::vector<std::string> &out) { 
    // construct a stream from the string 
    std::stringstream ss(str); 
 
    std::string s; 
    while (std::getline(ss, s, delim)) { 
        out.push_back(s); 
    } 
} 

int VGetIndex(std::vector<std::string> v, std::string K) {
    auto it = find(v.begin(), v.end(), K);
    // If element was found
    if (it != v.end()) {
        // calculating the index
        // of K
        int index = it - v.begin();
        return index;
    } else {
        // If the element is not
        // present in the vector
        return -1;
    }
}

// TODO added colour printing functions

