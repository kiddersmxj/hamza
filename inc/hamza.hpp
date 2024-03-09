#ifndef KHAMZA
#define KHAMZA

#include "config.hpp"
#include "child.hpp"
#include "assimilate.hpp"
#include "commands.hpp"
#include "execute.hpp"

#include <std-k.hpp>
#include <fstream>
#include <iostream>
#include <thread>
#include <getopt.h>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json_abi_v3_11_2::json;

void Usage();
void Usage(std::string Message);
void PrintVersion();
void FRInit();
void DoFR();
void LoadCommands(std::vector<JCommand> &Commands);
/* json Merge(const json &a, const json &b); */

#endif
