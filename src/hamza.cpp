#include "../inc/hamza.hpp"
#include <cstdlib>
#include <sched.h>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    int HelpFlag = 0;
    int VersionFlag = 0;
    int FRFlag = 0;
    int EncodeFlag = 0;
    std::string Parse = "";
    int opt;

    // Get opt option defenitions
    struct option Opts[] = {
        { "help", no_argument, &HelpFlag, 1 },
        { "version", no_argument, &VersionFlag, 1 },
        { "face-rec", no_argument, &FRFlag, 1 },
        { "encode", no_argument, &EncodeFlag, 1 },
        { "parse", required_argument, NULL, 'p' },
        { 0 }
    };

    // Infinite loop, to be broken when we are done parsing options
    while (1) {
        opt = getopt_long(argc, argv, "hvfep:", Opts, 0);

        // A return value of -1 indicates that there are no more options
        if (opt == -1) {
            if(HelpFlag && VersionFlag) {
                Usage();
                return EXIT_FAILURE;
            }
            break;
        } 

        switch (opt) {
        case 'h':
            HelpFlag = 1;
            break;
        case 'v':
            VersionFlag = 1;
            break;
        case 'f':
            FRFlag = 1;
            break;
        case 'e':
            EncodeFlag = 1;
            break;
        case 'p':
            Parse = optarg;
            break;
        case '?':
            Usage();
            return EXIT_FAILURE;
        default:
            Usage();
            break;
        }
    }

    if(HelpFlag) {
        Usage();
        return EXIT_SUCCESS;
    }
    if(VersionFlag) {
    }

    if(FRFlag) {
        DoFR();
    }

    if(EncodeFlag) {
        std::vector<Command> Commands;
        LoadCommands(Commands);

        k::Time Time;
        Assimilate Meaning(Commands);
        std::cout << Time.Close() << "ms" << std::endl;

        if(Parse == "") {
            /* std::cout << Meaning.Compare("called", "with the name of") << std::endl; */
            /* std::cout << Meaning.Attribute("turn off the lights") << std::endl; */
            /* std::cout << Meaning.Attribute("create a cpp project called assimilate") << std::endl; */
            /* std::cout << Meaning.Attribute("create a cpp program named assimilate") << std::endl; */
            /* std::cout << Meaning.Attribute("add a cpp class called assimilate") << std::endl; */
            /* std::cout << Meaning.Attribute("add a class file named assimilate of type cpp") << std::endl; */
            std::string Corpus;
            std::cout << "Enter corpus:" << std::endl;
            getline(std::cin, Corpus);
            while(Corpus != "") {
                auto CMD = Meaning.Attribute(Corpus);
                std::cout << CMD << std::endl;
#ifndef TEST
                Execute Execute(CMD);
#endif
                std::cout << std::endl << "Enter corpus:" << std::endl;
                getline(std::cin, Corpus);
            }
        } else  {
            auto CMD = Meaning.Attribute(Parse);
            std::cout << CMD << std::endl;
#ifndef TEST
                Execute Execute(CMD);
#endif
        }
        return EXIT_SUCCESS;
    }

    Usage();
    return EXIT_FAILURE;
}

void Usage() {
    std::cout << UsageNotes << std::endl;
}

void Usage(std::string Message) {
    std::cout << Message << std::endl;
    std::cout << UsageNotes << std::endl;
}

void PrintVersion() {
    std::cout << ProgramName << ": version " << Version << std::endl;
}

void FRInit() {
    std::ofstream fs("/tmp/shutdown.hamza", std::ofstream::trunc);
    fs << "0";
    fs.close();
}

void DoFR() {
    const char* Program = FRcmd.c_str();
    FRInit();
    Child FR(Program);
    while(!FR.QuestionExit()) {
        std::cout << FR.Read() << std::endl;
        k::BreakPoint();
    }
    FR.Close();
}

void LoadCommands(std::vector<Command> &Commands) {
    json Cmds;
    int i(0);
    for (const auto & File : fs::directory_iterator(CommandsDir)) {
        /* std::cout << File.path() << std::endl; */
        std::ifstream JFile(File.path());
        nlohmann::ordered_json JCmd = nlohmann::ordered_json::parse(JFile);
        for (nlohmann::ordered_json::iterator it = JCmd.begin(); it != JCmd.end(); ++it) {
            Command C;
            C.Name = it.key();
            C.Index = JCmd[C.Name]["index"];
            C.Confirm = JCmd[C.Name]["confirm"];
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
                std::vector<std::string> O;
                for(auto o: Options)
                    O.push_back(o);
                C.Options.push_back(O);
            }
            for(std::string Flag: JCmd[C.Name]["flags"])
                C.Flags.push_back(Flag);
            for(std::string DefaultFlag: JCmd[C.Name]["defaults"])
                C.DefaultFlags.push_back(DefaultFlag);

            /* std::cout   << "Name: " << C.Name */
            /*             << " Index: " << C.Index */ 
            /*             << " Confirm: " << C.Confirm */ 
            /*             << std::endl; */
            /* std::cout << "Bases: " << std::endl; */
            /* k::VPrint(C.Bases); */
            /* std::cout << "Args1: \n"; */
            /* k::VPrint(C.Args.at(0)); */
            /* std::cout << "Args2: \n"; */
            /* k::VPrint(C.Args.at(1)); */
            /* std::cout << "Opts1: \n"; */
            /* k::VPrint(C.Options.at(0)); */
            /* std::cout << "Opts2: \n"; */
            /* k::VPrint(C.Options.at(1)); */
            /* std::cout << "Flags: \n"; */
            /* k::VPrint(C.Flags); */
            /* std::cout << "DefaultFlags: \n"; */
            /* k::VPrint(C.DefaultFlags); */

            Commands.push_back(C);
        }
    }
}

/* json Merge(const json &a, const json &b) { */
/*     json result = a.flatten(); */
/*     json tmp = b.flatten(); */

/*     for (json::iterator it = tmp.begin(); it != tmp.end(); ++it) */
/*         result[it.key()] = it.value(); */

/*     return result.unflatten(); */
/* } */
