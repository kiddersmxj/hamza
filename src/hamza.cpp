#include "../inc/hamza.hpp"

int main(int argc, char** argv) {
    const char* Program = FRcmd;
    Init();

    Child FR(Program);
    while(!FR.QuestionExit()) {
        std::cout << FR.Read() << std::endl;
        BreakPoint();
    }

    FR.Close();
}

void Init() {
    std::ofstream fs("/tmp/shutdown.hamza", std::ofstream::trunc);
    fs << "0";
    fs.close();
}
