#!/bin/bash

set -e
Built=0

HamzaDir="$HOME/.hamza"

err() {
    rm -rf $HamzaDir/venv
    echo ""
    echo "An error occurred:"
    awk 'NR>L-4 && NR<L+4 { printf "%-5d%3s%s\n",NR,(NR==L?">>>":""),$0 }' L=$1 $0
}

trap 'err $LINENO' ERR

if [ -d "$HamzaDir/venv" ]; then
    if [[ -z "$VIRTUAL_ENV" ]]; then
        echo "No VIRTUAL_ENV set"
        echo "Please run . $HamzaDir/venv/bin/activate"
        exit
    else
        echo "VIRTUAL_ENV is set"
    fi
else
    echo "Building virtual environment"
    mkdir -p $HamzaDir
    python -m venv $HamzaDir/venv
    . $HamzaDir/venv/bin/activate
    echo "Installing requirments"
    python -m pip install --upgrade pip
    python -m pip install -r requirements.txt
    python -m pip install -U spacy
    python -m spacy download en_core_web_md
    echo "Requirements installed!"
    echo "Installing command files"
    cp -r commands $HamzaDir
    echo "Beginning compilation"
    Built=1
fi

if [[ $1 != "" ]]; then
    ARGS="-D$1"
fi

cmake -B build $ARGS && cmake --build build && sudo cmake --install build

if [[ $Built == 1 ]]; then
    echo ""
    echo "Please run . $HamzaDir/venv/bin/activate"
fi

# Copyright (c) 2023, Maxamilian Kidd-May
# All rights reserved.

# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree. 
