#include "../inc/assimilate.hpp"
#include <spacy/doc.h>
#include <vector>

Assimilate::Assimilate(std::vector<Command> &Cmds) : spacy(), \
                                        nlp(spacy.load("en_core_web_md")), \
                                        Commands(Cmds) {
    for(Command &C: Commands) {
        std::vector<Spacy::Doc> TmpDocs;
        for(std::string &B: C.Bases) {
            /* std::cout << "parse:" << C.Name << std::endl; */
            TmpDocs.push_back(nlp.parse(B));
        }
        C.BaseDocs = TmpDocs;
    }
}

Assimilate::~Assimilate() {
    for(auto &C: Commands)
        C.BaseDocs.clear();
}

double Assimilate::Compare(std::string One, std::string Two) {
    double Similarity;
    auto Doc1 = nlp.parse(One);
    auto Doc2 = nlp.parse(Two);
    Similarity = Doc1.similarity(Doc2);
    /* std::cout << doc1.similarity(doc2) << std::endl; */
    return Similarity;
}

std::string Assimilate::Attribute(std::string Input) {
    k::Time Time;

    /* RemoveStopwords(Input); */
    auto In = nlp.parse(Input);

    int Index;
    int CNum;
    std::string Name;
    double Highest = 0;
    std::string RootClause;
    std::string RootBase;
    std::vector<std::string> PassingArgs;
    std::vector<std::string> PassingFlags;
    Spacy::Token RootToken = GetRootClause(In, RootClause);
    std::vector<std::string> Clauses;
    std::vector<Spacy::Token> Tokens = GetClauses(In, Clauses);

    /* for(std::string Clause: Clauses) { // Could take a while, maybe just use Clauses.at(0) - or clause with root if slow! */
    /* std::cout << "For: " << RootClause << std::endl; */
    auto doc = nlp.parse(RootClause);
    int t(0);
    for(Command C: Commands) {
        int b(0);
        for(Spacy::Doc B: C.BaseDocs) {
            double Sim = doc.similarity(B);
            /* std::cout << C.Name << " " << Sim << std::endl; */
            if(Sim > Highest && Sim > MinSim) {
                Highest = Sim;
                Index = C.Index;
                Name = C.Name;
                RootBase = Commands.at(t).Bases.at(b);
                CNum = t;
            }
            if(Highest == 1) break;
            b++;
        }
        if(Highest == 1) break;
        t++;
    }
    if(Highest == 0) {
        std::cout << "Unkown Command" << std::endl;
        return "";
    }

    Command C = Commands.at(CNum);
    /* std::cout << "Using program: " << C.Name */
              /* << std::endl << "Clauses:" << std::endl; */
    /* k::VPrint(Clauses); */
    /* std::cout << "tokens:\n"; */
    for(auto t: Tokens)
        /* std::cout << t.text() << std::endl; */
    /* std::cout << std::endl; */

    bool Scanned = 0;
    int c(0);
    for(std::string Clause: Clauses) {
        /* std::cout << "\nnew clause\n\n"; */
        if(Clause == RootClause) {
            RemoveBase(RootBase, Clause);
        }
        if(k::RemoveWhitespace(Clause) != "") {
            auto ClauseDoc = nlp.parse(Clause);
            int t(0);
            double ArgHighest = 0;
            int ArgIndexTop = 0;
            int ArgIndexBtm = 0;
            for(std::vector<std::string> Arg: C.Args) {
                int b(0);
                if(C.Options.at(t).at(0) == "optarg") {
                    /* std::cout << "this has optarg" << std::endl; */
                    for(std::string ArgBase: Arg) {
                        double Sim = ClauseDoc.similarity(nlp.parse(ArgBase));
                        /* std::cout << "ArgNo: " << t << "/" << b << " Clause: \"" */ 
                                  /* << Clause << "\" AB: \"" */ 
                                  /* << ArgBase <<  "\" sim: " << Sim << std::endl; */
                        if(Sim > MinSim) {
                            if(Sim > ArgHighest) {
                                ArgHighest = Sim;
                                ArgIndexTop = t;
                                ArgIndexBtm = b;
                            }
                        }
                    }
                    b++;

                } else {
                }

                t++;
            }

            if(ArgHighest > 0) {
                /* std::cout << std::endl << "Clause: \"" << Clause << "\", Arg: " */ 
                          /* << C.Args.at(ArgIndexTop).at(ArgIndexBtm) */ 
                          /* << ", Score: " << ArgHighest << ", top: " << ArgIndexTop */ 
                          /* << ", btm: " << ArgIndexBtm << std::endl; */
                if(C.Options.at(ArgIndexTop).at(0) == "optarg") {
                    /* std::cout << "optarg!" << std::endl; */
                    if(Tokens.at(c).children().size() == 1) {
                        PassingArgs.push_back(Tokens.at(c).children().at(0).text());
                        PassingFlags.push_back(C.Flags.at(ArgIndexTop));
                    } else { // Not a real fix TODO make better
                        PassingArgs.push_back(Tokens.at(c).children().at(0).text());
                        PassingFlags.push_back(C.Flags.at(ArgIndexTop));
                    }
                }
                /* std::cout << std::endl; */
            }
        }
        c++;
    }

    t = 0;
    std::vector<Spacy::Doc> WordDocs;
    std::vector<std::string> Words;
    std::vector<std::string> PassingNoOptFlags;
    k::SplitString(Input, ' ', Words);
    /* k::VPrint(Words); */
    for(std::string Word: Words)
        WordDocs.push_back(nlp.parse(Word));
    for(auto Opt: C.Options) {
        double OptHighest = 0;
        int OptIndexTop = 0;
        int OptIndexBtm = 0;
        if(C.Options.at(t).at(0) == "optarg") {
        } else if(Opt.at(0) == ".") {
            double Sim = FindInWords(C.Args.at(t).at(0), WordDocs);
            if(Sim > HighSim) {
                /* std::cout << C.Args.at(t).at(0) << " wins with = " << HighSim << std::endl; */
                PassingNoOptFlags.push_back(C.Flags.at(t));
            }
                // Here is oportune to add possible options to request confirmation
        } else {
            for(std::string Opt: C.Options.at(t)) {
                std::string Arg;
                double Sim = FindInWords(Opt, WordDocs, Arg);
                if(Sim > HighSim) {
                    /* std::cout << Opt << " wins with = " << HighSim << std::endl; */
                    /* PassingNoOptFlags.push_back(C.Flags.at(t)); */
                    PassingArgs.push_back(Arg);
                    PassingFlags.push_back(C.Flags.at(t));
                    break; // might be a bad idea
                } // Here is oportune to add possible options to request confirmation
            }
        }
        t++;
    }

    /* std::cout << "Program: \"" << C.Name << "\"" << std::endl << "with args:" << std::endl; */
    /* k::VPrint(PassingArgs); */
    /* std::cout << "with arg flags:" << std::endl; */
    /* k::VPrint(PassingFlags); */
    /* std::cout << "with no arg flags:" << std::endl; */
    /* k::VPrint(PassingNoOptFlags); */

    /* std::cout << std::endl */
    /*           << "Passargs: " << PassingArgs.size() */ 
    /*           << " NooptF:" << PassingNoOptFlags.size() */ 
    /*           << " PassF: " << PassingFlags.size() << std::endl << std::endl; */

    std::string CMD = C.Command;
    for(int i(0); i<PassingArgs.size(); i++)
        CMD = CMD + " " + PassingFlags.at(i) + " " + PassingArgs.at(i); // need to add distinction between flags that tak args and dont so iterators line up
    for(std::string Flag: PassingNoOptFlags)
        CMD = CMD + " " + Flag;
    for(std::string Flag: C.DefaultFlags)
        CMD = CMD + " " + Flag;

    /* std::cout << "COMMAND: " << CMD << std::endl; */

    /* Time.Close(); */
    return CMD;
}

Spacy::Token Assimilate::GetRootClause(Spacy::Doc doc, std::string &Clause) {
    for (const auto& token: doc.tokens()) {
        std::vector<std::string> children;
        std::vector<std::string> ancestors;
        PopulateFamily(token, children, ancestors);
        
        /* DisplayParts(token, children, ancestors); */

        /* std::cout << token.dep_() << "-" << token.text() << std::endl; */

        if(token.dep_() == "ROOT") {
            /* std::cout << "v: " << token.text() << std::endl; */
            Clause = token.text();
            for(auto Part: token.children()) {
                /* std::cout << "C: " << Part.text() << std::endl; */
                Clause = Clause + " " + Part.text();
            }
            auto doc1 = nlp.parse(Clause);
            return token;
        }
    }
}

std::vector<Spacy::Token> Assimilate::GetClauses(Spacy::Doc doc, std::vector<std::string> &Clauses) {
    std::vector<Spacy::Token> tokens;
    std::vector<std::string> Verbs;
    for (const auto& token: doc.tokens()) {

        std::vector<std::string> children;
        std::vector<std::string> ancestors;
        PopulateFamily(token, children, ancestors);
        
        /* DisplayParts(token, children, ancestors); */

        /* std::cout << token.pos_() << "-" << token.text() << std::endl; */

        if(token.pos_() == "VERB") {
            /* std::cout << "v: " << token.text() << std::endl; */
            Verbs.push_back(token.text());
            std::string Clause = token.text();
            for(auto Part: token.children()) {
                /* std::cout << "C: " << Part.text() << std::endl; */
                Clause = Clause + " " + Part.text();
            }
            tokens.push_back(token);
            auto doc1 = nlp.parse(Clause);
            Clauses.push_back(Clause);
        }
    }
    return tokens;
}
void Assimilate::DisplayParts(Spacy::Token token, std::vector<std::string> children, \
                        std::vector<std::string> ancestors) {
        std::cout << token.text() << "\t" << token.i() << "\t"
                  << token.pos_() << "\t" << token.dep_() << "\t"
                  << "[";
        for (size_t i = 0; i < ancestors.size(); ++i) {
            std::cout << ancestors[i];
            if (i != ancestors.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << "\t" << "[";
        for (size_t i = 0; i < children.size(); ++i) {
            std::cout << children[i];
            if (i != children.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
}

void Assimilate::PopulateFamily(Spacy::Token token, std::vector<std::string> &children, \
                        std::vector<std::string> &ancestors) {
        for (const auto& t : token.ancestors())
            ancestors.push_back(t.text());
        for (const auto& t : token.children())
            children.push_back(t.text());
}

void Assimilate::RemoveBase(std::string Base, std::string &Clause) {
    // construct a stream from the string 
    std::stringstream ss(Base);
 
    std::string B; 
    while (std::getline(ss, B, ' ')) { 
        std::string::size_type i = Clause.find(B);

        if (i != std::string::npos)
           Clause.erase(i, B.length());
    }
}

void Assimilate::RemoveStopwords(std::string &sentence) {
    // Create a lambda function to check if a given word is a stopword
    auto isStopword = [&](const std::string& word) {
        return std::find(Stopwords.begin(), Stopwords.end(), word) != Stopwords.end();
    };

    // Split the sentence into words
    std::vector<std::string> words;
    size_t startPos = 0;
    size_t foundPos = sentence.find_first_of(' ');
    while (foundPos != std::string::npos) {
        words.push_back(sentence.substr(startPos, foundPos - startPos));
        startPos = foundPos + 1;
        foundPos = sentence.find_first_of(' ', startPos);
    }
    words.push_back(sentence.substr(startPos));

    // Remove stopwords from the words vector
    words.erase(std::remove_if(words.begin(), words.end(), isStopword), words.end());

    // Reconstruct the sentence without stopwords
    sentence.clear();
    for (const std::string& word : words) {
        sentence += word + ' ';
    }

    // Remove the trailing space, if any
    if (!sentence.empty()) {
        sentence.pop_back();
    }
}

double Assimilate::FindInWords(std::string Opt, std::vector<Spacy::Doc> WordDoc) {
    double Highest = 0;
    auto OptDoc = nlp.parse(Opt);
    for(Spacy::Doc doc: WordDoc) {
        double Sim = doc.similarity(OptDoc);
        /* std::cout << "Finding: " << Opt << " In Word: " << doc.text() << " = " << Sim << std::endl; */
        if(Sim > Highest)
            Highest = Sim;
    }
    return Highest;
}

double Assimilate::FindInWords(std::string Opt, std::vector<Spacy::Doc> WordDoc, std::string &Return) {
    double Highest = 0;
    auto OptDoc = nlp.parse(Opt);
    for(Spacy::Doc doc: WordDoc) {
        double Sim = doc.similarity(OptDoc);
        /* std::cout << "Finding: " << Opt << " In Word: " << doc.text() << " = " << Sim << std::endl; */
        if(Sim > Highest) {
            Highest = Sim;
            Return = doc.text();
        }
    }
    return Highest;
}

// Copyright (c) 2023, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 
