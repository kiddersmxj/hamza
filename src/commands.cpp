#include "../inc/commands.hpp"

#include <utility>
#include <spacy/doc.h>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_PRESERVE_ORDER
using json = nlohmann::json_abi_v3_11_2::json;

Commands::Commands(Spacy::Spacy &spacy, Spacy::Nlp &nlp) : spacy(spacy), nlp(nlp) {
    Create();
}

void Commands::Create() {
    // Creates a command for each file found (each cmd needs its own file)
    for (const auto & File : fs::directory_iterator(CommandsDir)) {
        // Print file locations
        /* std::cout << File.path() << std::endl; */
        std::ifstream JFile(File.path());
        nlohmann::ordered_json JCmd = nlohmann::ordered_json::parse(JFile);

        JCommand C;

        // This section sets up a struct called JCommand C that then populates our actual Commands matrix
        for(nlohmann::ordered_json::iterator it = JCmd.begin(); it != JCmd.end(); ++it) {
            C.Name = it.key();
            C.Index = JCmd[C.Name]["index"]; // Not used or currently reliable
            C.Confirm = JCmd[C.Name]["confirm"];
            C.Read = JCmd[C.Name]["read"];
            C.Command = JCmd[C.Name]["command"];
            for(std::string Base: JCmd[C.Name]["bases"])
                C.Bases.push_back(Base);
            for(auto Args: JCmd[C.Name]["args"]) {
                std::vector<std::string> A;
                for(auto a: Args)
                    A.push_back(a);
                C.Args.push_back(A);
            }

            for(auto Options: JCmd[C.Name]["options"]) {
                /* k::VPrint(Options); */
                std::vector<std::string> O;
                for(auto o: Options) {
                    O.push_back(o);
                    /* std::cout << o << std::endl; */
                }
                C.Options.push_back(O);
            }
            for(std::string Flag: JCmd[C.Name]["flags"])
                C.Flags.push_back(Flag);
            for(std::string DefaultFlag: JCmd[C.Name]["defaults"])
                C.DefaultFlags.push_back(DefaultFlag);
        }

        // Begin main matrix population
        // Adds the command bases
        int i(0);
        std::vector<std::pair<std::string, std::string>> Bases;
        for(auto Base: C.Bases) {
            std::pair<std::string, std::string> P;
            P.first = Base; P.second = C.Command;
            for(std::string F: C.DefaultFlags) {
                P.second = P.second + " " + F;
            }
            Bases.push_back(P);
            i++;
        }

        int j(0);
        std::vector<ArgGroup> CreatedArgs;
        for(auto Arg: C.Args) {
            ArgGroup ArgGroup;

            // Vector for storing possible options
            std::vector<std::string> Opts;
            // Gets the actual option from JCommand, eg. optarg, listarg, etc...
            std::string Option = C.Options.at(j).at(0);

            // Check the option and parse it (optarg, listarg, '.', etc...)
            if(Option == "listarg") {
                // Runs the arg flag with the list arg to get possible options
                // Need to work on if list is dependent on previous flag eg open -p optarg -f list
                std::string CMD = C.Command + " " + C.Flags.at(j) + " list";
                Child CheckListarg(CMD.c_str());
                while(!CheckListarg.QuestionExit()) {
                    Opts.push_back(k::StripTrailingNL(CheckListarg.Read()));
                }
                Opts.pop_back(); // Remove EXIT (last return from child.read()

                // Define main aspects of ArgGroup
                ArgGroup = { .OptionType = Option, .Base = Arg.at(0), .BaseFlag = C.Flags.at(j) };

                // This is for creation of an arg group (for listargs this will have multiple Arg structs in a vector
                // For optargs this will just be one Arg per ArgSubGroup
                for(auto A: Arg) {
                    ArgSubGroup SubGroup = { .OptionType = Option, .Base = A , .BaseFlag = C.Flags.at(j) };
                    for(auto O: Opts) {
                        std::string Output = A + " " + O;

                        ::Arg Arg = { .OptionType = Option, .String = Output, \
                            .Flag = C.Flags.at(j) + " " + O, .Base = A, .Doc = nlp.parse(Output) };
                        /* CreatedA.String = Output; */
                        /* CreatedA.OptionType = Option; */
                        /* CreatedA.Base = A; */
                        /* CreatedA.Flag = C.Flags.at(j) + " " + O; */

                        //need to add ability for args to not be exclusive (open on tag and destroy window eg)

                        SubGroup.Args.push_back(Arg);
                    }
                    ArgGroup.SubGroup.push_back(SubGroup);
                }
                CreatedArgs.push_back(ArgGroup);
                j++;

            } else if(Option == "optarg") {
                Opts.push_back("optarg");

                // Define main aspects of ArgGroup
                ArgGroup = { .OptionType = Option, .Base = Arg.at(0), .BaseFlag = C.Flags.at(j) };

                // This is for creation of an arg group (for listargs this will have multiple Arg structs in a vector
                // For optargs this will just be one Arg per ArgSubGroup
                for(auto O: Opts) {
                    ArgSubGroup SubGroup = { .OptionType = Option, .Base = Arg.at(0) , .BaseFlag = C.Flags.at(j) };
                    for(auto A: Arg) {
                        std::string Output = A;

                        ::Arg Arg = { .OptionType = Option, .String = Output, \
                            .Flag = C.Flags.at(j) + " " + O, .Base = A, .Doc = nlp.parse(Output) };
                        /* CreatedA.String = Output; */
                        /* CreatedA.OptionType = Option; */
                        /* CreatedA.Base = A; */
                        /* CreatedA.Flag = C.Flags.at(j) + " " + O; */

                        //need to add ability for args to not be exclusive (open on tag and destroy window eg)

                        SubGroup.Args.push_back(Arg);
                    }
                    ArgGroup.SubGroup.push_back(SubGroup);
                }
                CreatedArgs.push_back(ArgGroup);
                j++;

            } else if(Option == ".") {
                Opts.push_back(".");

                // Define main aspects of ArgGroup
                ArgGroup = { .OptionType = Option, .Base = Arg.at(0), .BaseFlag = C.Flags.at(j) };

                // This is for creation of an arg group (for listargs this will have multiple Arg structs in a vector
                // For optargs this will just be one Arg per ArgSubGroup
                for(auto A: Arg) {
                    ArgSubGroup SubGroup = { .OptionType = Option, .Base = A , .BaseFlag = C.Flags.at(j) };
                    for(auto O: Opts) {
                        std::string Output = A;

                        ::Arg Arg = { .OptionType = Option, .String = Output, \
                            .Flag = C.Flags.at(j), .Base = A, .Doc = nlp.parse(Output) };
                        /* CreatedA.String = Output; */
                        /* CreatedA.OptionType = Option; */
                        /* CreatedA.Base = A; */
                        /* CreatedA.Flag = C.Flags.at(j) + " " + O; */

                        //need to add ability for args to not be exclusive (open on tag and destroy window eg)

                        SubGroup.Args.push_back(Arg);
                    }
                    ArgGroup.SubGroup.push_back(SubGroup);
                }
                CreatedArgs.push_back(ArgGroup);
                j++;


            } else {
                Opts.push_back(Option);
                Option = "else";

                // Define main aspects of ArgGroup
                ArgGroup = { .OptionType = Option, .Base = Arg.at(0), .BaseFlag = C.Flags.at(j) };

                // This is for creation of an arg group (for listargs this will have multiple Arg structs in a vector
                // For optargs this will just be one Arg per ArgSubGroup
                for(auto A: Arg) {
                    ArgSubGroup SubGroup = { .OptionType = Option, .Base = A , .BaseFlag = C.Flags.at(j) };
                    for(auto O: Opts) {
                        std::string Output = A + " " + O;

                        ::Arg Arg = { .OptionType = Option, .String = Output, \
                            .Flag = C.Flags.at(j) + " " + O, .Base = A, .Doc = nlp.parse(Output) };
                        /* CreatedA.String = Output; */
                        /* CreatedA.OptionType = Option; */
                        /* CreatedA.Base = A; */
                        /* CreatedA.Flag = C.Flags.at(j) + " " + O; */

                        //need to add ability for args to not be exclusive (open on tag and destroy window eg)

                        SubGroup.Args.push_back(Arg);
                    }
                    ArgGroup.SubGroup.push_back(SubGroup);
                }
                CreatedArgs.push_back(ArgGroup);
                j++;


            }

        }
        Add(C.Name, CreatedArgs, Bases, C.DefaultFlags);
    }

    // Very useful debug cout

/*     std::cout << "-----------------------------\n"; */
/*     for(auto C: GetCommandsMaster()) { */
/*         std::cout << "\n--------" << C.Name << "--------\n"; */
/*         /1* std::cout << C.BaseSentence.String << std::endl; *1/ */
/*         for(auto Sen: C.BaseSentences) { */
/*             std::cout << Sen.String << std::endl; */
/*         } */
/*         std::cout << "\nBases\n"; */
/*         for(auto B: C.Bases) { */
/*             std::cout << "\t" << B << std::endl; */
/*         } */
/*         std::cout << std::endl; */
/*         for(auto ArgGroup: C.Args) { */
/*             std::cout << "ArgGroup: " << ArgGroup.Base << " (" << ArgGroup.OptionType << ") " << ArgGroup.BaseFlag << "\n"; */
/*             for(auto Subgroup: ArgGroup.SubGroup) { */
/*                 for(auto Arg: Subgroup.Args) { */
/*                     std::cout << "\t" << Arg.String << "\t\t\t (" << Arg.Flag << ")" << std::endl; */
/*                 } */
/*             } */
/*         } */
/*     } */

    /* for(auto C: GetCommandsMaster()) { */
    /*     for(auto S: C.Sentences) { */
    /*         Sentences.push_back(S); */
    /*     } */
    /* } */

    /* std::cout << "-------------Sentences-------------\n"; */
    /* for(auto S: Sentences) { */
    /*     std::cout << S.String << " (" << S.Command << ")\n"; */
    /* } */

}

// the heirachy does not goes Args--|
//                                  |-Arg-|-A-A-A-A
//                                  |     |
//                                  |     |-Doc-Flag-String-OptionType
//                                  |     
//                                  |-Arg-|-A-A-A-A
void Commands::Add(std::string Name, std::vector<ArgGroup> Args, std::vector<std::pair<std::string, std::string>> Bases, \
        std::vector<std::string> DefaultFlags) {

    // Creating base sentences
    // Sentence bases are the overall base for the prelim scan detailed in the next block
    std::vector<std::string> SentenceBases;
    // Sections are like the args that will get added together later for every variation
    std::vector<std::vector<std::pair<std::string, std::string>>> SentenceSections;
    std::vector<std::string> Optargs;
    for(ArgGroup ArgGroup: Args) {
        SentenceBases.push_back(ArgGroup.Base);

        // Complex Sentences Builder beginning (creats the sections)
        std::vector<std::pair<std::string, std::string>> Section;
        for(auto SubGroup: ArgGroup.SubGroup) {
            /* std::cout << "SubGroup: " << SubGroup.Base << " " << SubGroup.OptionType << std::endl; */
            if(SubGroup.OptionType == "optarg") {
                for(auto Arg: SubGroup.Args) {
                    std::pair<std::string, std::string> Pair; Pair.first = Arg.String; Pair.second = Arg.Flag;
                    std::string Optarg = Arg.String;
                    Section.push_back(Pair);
                    Optargs.push_back(Optarg);
                    /* std::cout << "\t" << Arg.String << std::endl; */
                }
                /* Section.push_back(SubGroup.Args.at(0).String); */
                /* std::cout << "\t" << SubGroup.Args.at(0).String << std::endl; */
            } else if(SubGroup.OptionType == "listarg") {
                for(auto Arg: SubGroup.Args) {
                    std::pair<std::string, std::string> Pair; Pair.first = Arg.String; Pair.second = Arg.Flag;
                    Section.push_back(Pair);
                    /* std::cout << "\t" << Arg.String << std::endl; */
                }
            } else if(SubGroup.OptionType == ".") {
                std::pair<std::string, std::string> Pair;
                Pair.first = SubGroup.Args.at(0).String; Pair.second = SubGroup.Args.at(0).Flag;
                Section.push_back(Pair);
                /* std::cout << "\t" << SubGroup.Args.at(0).String << std::endl; */
            } else { std::cout << "************WEIRD ELSE************\n"; }
            /* std::cout << "t: " << ArgGroup.SubGroup.at(ArgGroup.SubGroup.size()-1).Base << std::endl; */
            if(SubGroup.Base == ArgGroup.SubGroup.at(ArgGroup.SubGroup.size() - 1).Base \
                    && SubGroup.Base != "") {
                /* std::cout << SubGroup.Base << std::endl; */
                SentenceSections.push_back(Section);
            }
        }

    }

    // Gets all the combos of the sentence bases so that we can estimate
    std::vector<std::string> Combinations = GetAllCombinations(SentenceBases);

    // Creates the base sentences (just takes the top layer of each arg to guage which cmd it may be
    // could defo use more work to make it more accurate
    std::vector<Sentence> BaseSentences;
    for(const auto& Combination : Combinations) {
        Sentence Sentence = { .Doc = nlp.parse(Bases.at(0).first + " " + Combination), \
            .String = Bases.at(0).first + " " + Combination };
        /* std::cout << Bases.at(0) << " " << Combination << std::endl; */
        BaseSentences.push_back(Sentence);
    }

    // uses the created arg vectors and adds them all together to get all variations
    std::vector<std::pair<std::string, std::string>> SentencePairs;
    SentencePairs = CreateSentencePairs(SentenceSections, Bases);

    // Creates the final sentences for each command
    double i(0);
    double Per(0);
    std::vector<Sentence> SentenceStructures;
    for(std::pair<std::string, std::string> P: SentencePairs) {
        Sentence S = { .Doc = nlp.parse(P.first), .String = P.first, .Command = P.second, .Optargs = Optargs };
        double NewPer = ((i/(SentencePairs.size()-1))*100);
        if(NewPer != Per) {
            k::WriteOnSameLine(std::to_string(NewPer) + "%");
        }
        Per = NewPer;
        SentenceStructures.push_back(S);
        i++;
    }
    std::cout << std::endl;

    Command Command = { .Name = Name, .BaseSentences = BaseSentences, .Sentences = SentenceStructures, \
        .Bases = Bases, .Args = Args };
    CommandsMaster.push_back(Command);
}

ArgGroup Commands::GetCommandArgGroup(int X, int Y) {
    return CommandsMaster.at(X).Args.at(Y);
}

/* Command Commands::GetCommand(std::string Name) { */
/*     Command C; */
/*     for(auto N: CommandsMaster) { */
/*     } */
/*     return C; */
/* } */

Command Commands::GetCommand(int Index) {
    return CommandsMaster.at(Index);
}

std::vector<Command> Commands::GetCommandsMaster() {
    return CommandsMaster;
}

std::vector<std::string> GetAllMatrixCombinations(const std::vector<std::vector<std::string>>& input) {
    // Vector to hold all combinations
    std::vector<std::string> Combinations;

    // Call the function to generate combinations
    std::vector<std::string> CurrentCombination;
    GenerateMatrixCombinations(input, CurrentCombination, Combinations);
    return Combinations;
}

// Function to recursively generate all combinations of strings
void GenerateMatrixCombinations(const std::vector<std::vector<std::string>>& input, std::vector<std::string>& current \
        , std::vector<std::string>& result, size_t index) {
    // Base case: if we have processed all input vectors, add the current combination to result
    if (index == input.size()) {
        std::string combination;
        for (const auto& str : current) {
            combination += str + " ";
        }
        combination.pop_back(); // Remove the extra space at the end
        result.push_back(combination);
        return;
    }

    // Recursively generate combinations for the current input vector
    for (const auto& str : input[index]) {
        current.push_back(str);
        GenerateMatrixCombinations(input, current, result, index + 1);
        current.pop_back(); // Backtrack to explore other combinations
    }
}

void GenerateCombinations(const std::vector<std::string>& strings, int index, std::string currentCombination, std::vector<std::string>& result) {
    if (index == strings.size()) {
        result.push_back(currentCombination);
        return;
    }

    // Case 1: Exclude the current string
    GenerateCombinations(strings, index + 1, currentCombination, result);

    // Case 2: Include the current string
    if (!currentCombination.empty()) {
        currentCombination += " "; // Add a space if not the first string
    }
    GenerateCombinations(strings, index + 1, currentCombination + strings[index], result);
}

std::vector<std::string> GetAllCombinations(const std::vector<std::string>& strings) {
    std::vector<std::string> result;
    GenerateCombinations(strings, 0, "", result);
    return result;
}

std::vector<std::pair<std::string, std::string>> Combine(std::vector<std::pair<std::string, std::string>> X, \
        std::vector<std::pair<std::string, std::string>> Y) {
    std::vector<std::pair<std::string, std::string>> Combination;
    for(std::pair<std::string, std::string> x: X) {
        Combination.push_back(x);
    }
    for(std::pair<std::string, std::string> y: Y) {
        Combination.push_back(y);
    }

    for(std::pair<std::string, std::string> x: X) {
        for(std::pair<std::string, std::string> y: Y) {
            std::pair<std::string, std::string> Pair;
            Pair.first = x.first + " " + y.first; Pair.second = x.second + " " + y.second;
            Combination.push_back(Pair);
        }
    }
    return Combination;
}

std::vector<std::pair<std::string, std::string>> CreateSentencePairs(std::vector<std::vector<std::pair<std::string, \
        std::string>>> SentenceSections, std::vector<std::pair<std::string, std::string>> Bases) {
    int i(0);
    std::vector<std::pair<std::string, std::string>> CurrentConcatinations;
    for(std::vector<std::pair<std::string, std::string>> Section: SentenceSections) {
        if(i == 0) {
            CurrentConcatinations = Section;
        } else {
            CurrentConcatinations = Combine(CurrentConcatinations, Section);
        }
        i++;
    }

    std::vector<std::pair<std::string, std::string>> FinalConcatinations;
    for(std::pair<std::string, std::string> P: CurrentConcatinations) {
        for(std::pair<std::string, std::string> B: Bases) {
            std::pair<std::string, std::string> F;
            F.first = B.first + " " + P.first;
            F.second = B.second + " " + P.second;
            /* std::cout << F.first << " (" << F.second << ")" << std::endl; */
            FinalConcatinations.push_back(F);
        }
    }

    return FinalConcatinations;
}

// Copyright (c) 2023, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

