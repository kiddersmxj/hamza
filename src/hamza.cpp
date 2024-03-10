#include "../inc/hamza.hpp"
#include <cstdlib>
#include <sched.h>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    int HelpFlag = 0;
    int VersionFlag = 0;
    int FRFlag = 0;
    int AssimilateFlag = 0;
    int TestFlag = 0;
    std::string Parse = "";
    int opt;

    // Get opt option defenitions
    struct option Opts[] = {
        { "help", no_argument, &HelpFlag, 1 },
        { "version", no_argument, &VersionFlag, 1 },
        { "face-rec", no_argument, &FRFlag, 1 },
        { "assimilate", no_argument, &AssimilateFlag, 1 },
        { "parse", required_argument, NULL, 'p' },
        { "test", no_argument, &TestFlag, 't' },
        { 0 }
    };

    // Infinite loop, to be broken when we are done parsing options
    while (1) {
        opt = getopt_long(argc, argv, "hvfap:t", Opts, 0);

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
        case 'a':
            AssimilateFlag = 1;
            break;
        case 'p':
            Parse = optarg;
            AssimilateFlag = 1;
            break;
        case 't':
            TestFlag = 1;
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

    if(TestFlag) {
        k::Time Time;
        Spacy::Spacy spacy;
        Spacy::Nlp nlp(spacy.load("en_core_web_md"));
        Commands Commands(spacy, nlp);
        Assimilate Assimilate(Commands, spacy, nlp);;
        std::cout << Time.Close() << "ms" << std::endl;
        /* Commands.Create(); */

        /* std::vector<Command> Commands; */
        /* LoadCommands(Commands); */
        /* CreateCommandIterations(); */
    }

    if(FRFlag) {
        DoFR();
    }

    if(AssimilateFlag) {
        k::Time Time;
        Spacy::Spacy spacy;
        Spacy::Nlp nlp(spacy.load("en_core_web_md"));
        Commands Commands(spacy, nlp);
        Assimilate Assimilate(Commands, spacy, nlp);;
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
                std::string CMD;
                int Read = Assimilate.Attribute(Corpus, CMD);
                if(Read != -1) {
                    std::cout
                            << std::endl
                            << "*************************" << std::endl
                            << CMD << std::endl
                            << "*************************" << std::endl
                            << std::endl;
#ifndef TEST
                    if(Read == 1) Execute Execute(CMD);
                    if(Read == 0) Child Child(CMD.c_str());
#endif
                }
                std::cout << std::endl << "Enter corpus:" << std::endl;
                getline(std::cin, Corpus);
            }
        } else  {
            std::string CMD;
            int Read = Assimilate.Attribute(Parse, CMD);
            if(Read != -1) {
                std::cout
                        << std::endl
                        << "*************************" << std::endl
                        << CMD << std::endl
                        << "*************************" << std::endl
                        << std::endl;
#ifndef TEST
                if(Read == 1) Execute Execute(CMD);
                if(Read == 0) Child Child(CMD.c_str());
#endif
            }
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

/* void LoadCommands(std::vector<Command> &Commands) { */
/*     int i(0); */
/*     for (const auto & File : fs::directory_iterator(CommandsDir)) { */
/*         /1* std::cout << File.path() << std::endl; *1/ */
/*         std::ifstream JFile(File.path()); */
/*         nlohmann::ordered_json JCmd = nlohmann::ordered_json::parse(JFile); */
/*         for (nlohmann::ordered_json::iterator it = JCmd.begin(); it != JCmd.end(); ++it) { */
/*             Command C; */
/*             /1* std::cout << JCmd[C.Name] << std::endl; *1/ */
/*             C.Name = it.key(); */
/*             C.Index = JCmd[C.Name]["index"]; // Not used or currently reliable */
/*             C.Confirm = JCmd[C.Name]["confirm"]; */
/*             C.Read = JCmd[C.Name]["read"]; */
/*             C.Command = JCmd[C.Name]["command"]; */
/*             for(std::string Base: JCmd[C.Name]["bases"]) */
/*                 C.Bases.push_back(Base); */
/*             for(auto Args: JCmd[C.Name]["args"]) { */
/*                 std::vector<std::string> A; */
/*                 for(auto a: Args) */
/*                     A.push_back(a); */
/*                 C.Args.push_back(A); */
/*             } */
/*             for(auto Options: JCmd[C.Name]["options"]) { */
/*                 std::vector<std::string> O; */
/*                 for(auto o: Options) */
/*                     O.push_back(o); */
/*                 C.Options.push_back(O); */
/*             } */
/*             for(std::string Flag: JCmd[C.Name]["flags"]) */
/*                 C.Flags.push_back(Flag); */
/*             for(std::string DefaultFlag: JCmd[C.Name]["defaults"]) */
/*                 C.DefaultFlags.push_back(DefaultFlag); */

/*             /1* std::cout   << "Name: " << C.Name *1/ */
/*             /1*             << " Index: " << C.Index *1/ */ 
/*             /1*             << " Confirm: " << C.Confirm *1/ */ 
/*             /1*             << std::endl; *1/ */
/*             /1* std::cout << "Bases: " << std::endl; *1/ */
/*             /1* k::VPrint(C.Bases); *1/ */
/*             /1* std::cout << "Args1: \n"; *1/ */
/*             /1* k::VPrint(C.Args.at(0)); *1/ */
/*             /1* std::cout << "Args2: \n"; *1/ */
/*             /1* k::VPrint(C.Args.at(1)); *1/ */
/*             /1* std::cout << "Opts1: \n"; *1/ */
/*             /1* k::VPrint(C.Options.at(0)); *1/ */
/*             /1* std::cout << "Opts2: \n"; *1/ */
/*             /1* k::VPrint(C.Options.at(1)); *1/ */
/*             /1* std::cout << "Flags: \n"; *1/ */
/*             /1* k::VPrint(C.Flags); *1/ */
/*             /1* std::cout << "DefaultFlags: \n"; *1/ */
/*             /1* k::VPrint(C.DefaultFlags); *1/ */

/*             Commands.push_back(C); */
/*         } */
/*     } */
/* } */

/* json Merge(const json &a, const json &b) { */
/*     json result = a.flatten(); */
/*     json tmp = b.flatten(); */

/*     for (json::iterator it = tmp.begin(); it != tmp.end(); ++it) */
/*         result[it.key()] = it.value(); */

/*     return result.unflatten(); */
/* } */
