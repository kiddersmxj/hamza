#!/bin/bash

if [[ $1 != "" ]]; then
    ARGS="-D$1"
fi

cmake -B build $ARGS && cmake --build build && sudo cmake --install build

# Copyright (c) 2023, Maxamilian Kidd-May
# All rights reserved.

# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree. 
