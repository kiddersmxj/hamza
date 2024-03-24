#ifndef K_COMMANDS
#define K_COMMANDS

#include "execute.hpp"
#include "config.hpp"

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
} JCommand;

typedef struct {
    std::string OptionType;
    std::string String;
    std::string Flag;
    std::string Base;
    Spacy::Doc Doc;
} Arg;
typedef struct {
    std::vector<Arg> Args;
    std::string OptionType;
    std::string Base;
    std::string BaseFlag;
} ArgSubGroup;
typedef struct {
    std::vector<ArgSubGroup> SubGroup;
    std::string OptionType;
    std::string Base;
    std::string BaseFlag;
} ArgGroup;
typedef struct {
    Spacy::Doc Doc;
    std::string String;
    std::string Command;
    std::vector<std::string> Optargs;
} Sentence;
typedef struct {
    std::string Name;
    std::vector<Sentence> BaseSentences;
    std::vector<Sentence> Sentences;
    std::vector<std::pair<std::string, std::string>> Bases;
    std::vector<ArgGroup> Args;
} Command;

class Commands {
    private:
        Spacy::Spacy spacy;
        Spacy::Nlp nlp;
        std::vector<Sentence> Sentences;
    public:
        std::vector<Command> CommandsMaster;
        Commands(Spacy::Spacy &spacy, Spacy::Nlp &nlp);
        void Create();
        void Add(std::string Name, std::vector<ArgGroup> Args, std::vector<std::pair<std::string, std::string>> Bases, \
                std::vector<std::string> DefaultFlags);
        Command GetCommand(std::string Name);
        Command GetCommand(int Index);
        std::vector<Command> GetCommandsMaster();
        ArgGroup GetCommandArgGroup(int X, int Y);
        void CreateSentenceMatrix();
};

std::vector<std::string> GetAllMatrixCombinations(const std::vector<std::vector<std::string>>& input);
void GenerateMatrixCombinations(const std::vector<std::vector<std::string>>& input, std::vector<std::string>& current \
        , std::vector<std::string>& result, size_t index = 0);
void GenerateCombinations(const std::vector<std::string>& strings, int index, \
        std::vector<std::string>& currentCombination, std::vector<std::vector<std::string>>& result);
std::vector<std::string> GetAllCombinations(const std::vector<std::string>& strings);
std::vector<std::pair<std::string, std::string>> Combine(std::vector<std::pair<std::string, std::string>> X, \
        std::vector<std::pair<std::string, std::string>> Y);
std::vector<std::pair<std::string, std::string>> CreateSentencePairs(std::vector<std::vector<std::pair<std::string, \
        std::string>>> SentenceSections, std::vector<std::pair<std::string, std::string>> Bases);

#endif

// Copyright (c) 2023, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

