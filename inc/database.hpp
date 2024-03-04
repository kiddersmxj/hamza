#ifndef K_DATABASE
#define K_DATABASE

#include "config.hpp"

#include <opus/opus.h>
#include <sndfile.h>
#include <vector>
#include <string>

class Database {
    public:
        Database();
        ~Database();
    private:
        bool CompressWavToOpus(const std::string& inputFileName, const std::string& outputFileName);
        bool DecompressOpusAndPlay(const std::string& inputFileName, double startTimestamp, double endTimestamp);
};

#endif

// Copyright (c) 2023, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

