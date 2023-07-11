#include "../inc/execute.hpp"

Execute::Execute(std::string Program) : Child((Program + " 2>&1; echo \"$?\"").c_str()) {
    int i(0);
    while(!QuestionExit()) {
        Read();
    }
    int Err = Close();
    if(Err == 0)
        return;

    std::cout << "errno:  " << Err << std::endl;
    if(Err == -1) {
        // NO info given on proc execution by pclose - report to user
    } else if(Err > 0) {
        std::vector<std::string> ErrMsg;
        k::SplitString(Output.front(), ':', ErrMsg);
        std::cout << "errstr: " << ErrMsg.back() << std::endl;
    }
}

// Copyright (c) 2023, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 
