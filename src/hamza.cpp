#include "../inc/hamza.hpp"

int main(int argc, char** argv) {
    const char* Program = FRcmd;

    Child FR(Program);
    while(!FR.QuestionExit()) {
        std::cout << FR.Read() << std::endl;
        BreakPoint();
    }

    FR.Close();
}
