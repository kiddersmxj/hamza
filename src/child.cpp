#include "../inc/child.hpp"

Child::Child(const char* Program) {
    std::cout << "Launching child process... " << Program << std::endl;

    fp = popen(Program, "r");
    if (fp == NULL) {
        /* Handle error */;
        std::cout << "popen error" << std::endl;
    }
}

std::string Child::Read() {
    /* std::cout << fgets(path, 1000, fp) << std::endl; */
    char* Out =  fgets(path, 1000, fp);
    if(Out != NULL) {
        Output.push_back(k::StripTrailingNL(Out));
        return Out;
    } else  {
        Exit = 1;
        return "EXIT";
    }
}

bool Child::QuestionExit() {
    return Exit;
}

int Child::Close() {
    int status = pclose(fp);
    if (status == -1) {
        /* Error reported by pclose() */
        printf("pclose error reported");
    } else {
        /* Use macros described under wait() to inspect `status' in order
         to determine success/failure of command executed by popen() */
        /* printf("Done running"); */
    }
    int Err = -1;
    std::string err = Output.back();
    if(k::IsInteger(err))
        Err = std::atoi(Output.back().c_str());
    else
        std::cout << "Invalid error code" << std::endl;

    /* std::cout << "Output\n"; */
    /* k::VPrint(Output); */
    /* std::cout << "Output\n"; */
    return Err;
}
