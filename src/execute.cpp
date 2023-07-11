#include "../inc/execute.hpp"

Execute::Execute(const char* Program) : Child(Program) {
    /* while(!QuestionExit()) { */
    /*     std::cout << "r: " << Read() << "r" << std::endl; */
    /* } */
    Close();
}

// Copyright (c) 2023, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 
