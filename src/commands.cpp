#include "../inc/commands.hpp"

#include <utility>
#include <spacy/doc.h>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_PRESERVE_ORDER
using json = nlohmann::json_abi_v3_11_2::json;

Commands::Commands(Spacy::Spacy spacy, Spacy::Nlp nlp) : spacy(spacy), nlp(nlp) {
    Create();
}

void Commands::Create() {
    /* Commands Commands; */
    for (const auto & File : fs::directory_iterator(CommandsDir)) {
        // Print file locations
        /* std::cout << File.path() << std::endl; */
        std::ifstream JFile(File.path());
        nlohmann::ordered_json JCmd = nlohmann::ordered_json::parse(JFile);

        JCommand C;

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
            /* std::cout << "\n---" << C.Name << "---" << std::endl */ 
            /*     << "options:" << std::endl; */

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

        int i(0);
        // Could porbably optimise by iterating through args first - test
        /* std::cout << "-------------" << std::endl */
        /*     << "bases:" << std::endl; */
        std::vector<std::string> Bases;
        for(auto Base: C.Bases) {
            /* std::cout << i << "-" << Base << std::endl; */
            Bases.push_back(Base);
            i++;
        }

        /* std::cout << "|-------------" << std::endl; */
        /* k::VPrint(Bases); */
        /* std::cout << "|-------------" << std::endl; */

        int j(0);
        std::vector<ArgGroup> CreatedArgs;
        for(auto Arg: C.Args) {
            std::vector<::Arg> CreatedArg;

            // Vector for storing possible options
            std::vector<std::string> Opts;
            std::string Option = C.Options.at(j).at(0);

            std::cout << "F: " << C.Flags.at(j) << std::endl;
            
            // Check the option and parse it (optarg, listarg, '.')
            /* std::cout << Option << std::endl; */
            if(Option == "listarg") {
                /* Opts.push_back("listarg"); */

                //not working properly rn
                std::string CMD = C.Command + " " + C.Flags.at(j) + " list";
                Child CheckListarg(CMD.c_str());
                while(!CheckListarg.QuestionExit()) {
                    Opts.push_back(k::StripTrailingNL(CheckListarg.Read()));
                }
                Opts.pop_back(); // Remove EXIT (last return from child.read()
            } else if(Option == "optarg") {
                // Add tag to grab words after key word
                Opts.push_back("optarg");
            } else if(Option == ".") {
                Opts.push_back(".");
            } else {
                Opts.push_back(Option);
                Option = "else";
            }

            // This is for creation of an arg group (for listargs this will have multiple Arg structs in a vector
            // For optargs this will just be one Arg per ArgGroup
            for(auto A: Arg) {
                ArgGroup ArgGroup = { .OptionType = Option, .Base = A , .BaseFlag = C.Flags.at(j) };
                for(auto O: Opts) {
                    std::string Output = A + " " + O;

                    ::Arg CreatedA = { .Doc = nlp.parse(Output) };
                    CreatedA.String = Output;
                    CreatedA.OptionType = Option;
                    CreatedA.Base = A;
                    CreatedA.Flag = C.Flags.at(j) + " " + O;

                    //need to add ability for args to not be exclusive (open on tag and destroy window eg)

                    /* std::cout << "CREATDA: " << CreatedA.String << std::endl; */
                    /* CreatedArg.push_back(CreatedA); */
                    ArgGroup.Args.push_back(CreatedA);
                }
                /* for(auto tmp: CreatedArg) { */
                /*     std::cout << "=====" << tmp.String << "=======" << std::endl; */
                /*     std::cout << "=====" << tmp.OptionType << "=======" << std::endl; */
                /* } */
                CreatedArgs.push_back(ArgGroup);
            }
            j++;
        }
        /* for(auto tmp: CreatedArgs) { */
        /*     for(auto t: tmp) { */
        /*         std::cout << "++++++++ " << t.String << std::endl; */
        /*     } */
        /* } */
        
        Add(C.Name, CreatedArgs, Bases);
    }

    // Very useful debug cout

    std::cout << "-----------------------------\n";
    for(auto C: GetCommandsMaster()) {
        std::cout << "\n--------" << C.Name << "--------\nBases:\n";
        for(auto B: C.Bases) {
            std::cout << "\t" << B << std::endl;
        }
        std::cout << std::endl;
        for(auto ArgGroup: C.Args) {
            std::cout << "Arg: " << ArgGroup.Base << " (" << ArgGroup.OptionType << ") " << ArgGroup.BaseFlag << "\n";
            for(auto Arg: ArgGroup.Args) {
                /* std::cout << "Arg: (" << Arg.at(0).OptionType << ")\n"; */
                /* for(auto A: Arg.String) { */
                    std::cout << "\t" << Arg.String << "\t\t\t (" << Arg.Flag << ")" << std::endl;
                /* } */
            }
        }
    }

}

// the heirachy goes Args-|
//                        |-Arg-|-A-A-A-A
//                        |       |
//                        |       |-Doc-Flag-String-OptionType
//                        |     
//                        |-Arg-|-A-A-A-A
void Commands::Add(std::string Name, std::vector<ArgGroup> Args, std::vector<std::string> Bases) {
    Command Command = { .Name = Name,.Bases = Bases, .Args = Args };
    CommandsMaster.push_back(Command);
}

ArgGroup Commands::GetCommandArgGroup(int X, int Y) {
    return CommandsMaster.at(X).Args.at(Y);
}

Command Commands::GetCommand(std::string Name) {
    Command C;
    for(auto N: CommandsMaster) {
    }
    return C;
}

Command Commands::GetCommand(int Index) {
    return CommandsMaster.at(Index);
}

std::vector<Command> Commands::GetCommandsMaster() {
    return CommandsMaster;
}

// Copyright (c) 2023, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

