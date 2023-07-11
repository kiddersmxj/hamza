#ifndef KCHILD
#define KCHILD
#include <iostream>
#include <stdio.h>
#include <std-k.hpp>

#define PATH_MAX 1000

class Child {
    public:
        Child(const char* Program);
        std::string Read();
        int Close();
        bool QuestionExit();
        std::vector<std::string> Output;
    private:
        bool Exit = 0;
        char path[PATH_MAX];
        FILE *fp;
};
#endif
