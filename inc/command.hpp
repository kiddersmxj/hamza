#ifndef Kcommandcommand
#define Kcommandcommand

#include <spacy/spacy>
#include <iostream>
#include <vector>

typedef struct {
    int Index;
    int Read;
    bool Confirm;
    std::string Name;
    std::string Command;
    std::vector<std::string> Bases;
    std::vector<Spacy::Doc> BaseDocs;
    std::vector<std::vector<std::string>> Args;
    std::vector<std::vector<std::string>> Options;
    std::vector<std::string> Flags;
    std::vector<std::string> DefaultFlags;
} Commandxx;

#endif

// Copyright (c) 2023, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 
