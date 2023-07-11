cmake_minimum_required (VERSION 3.26)
 
# --- Fetch libs --------------------------------------------------------------

include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(std-k
  GIT_REPOSITORY https://github.com/kiddersmxj/std-k
  GIT_TAG main
)
 
FetchContent_GetProperties(std-k)
if(NOT std-k_POPULATED)
  FetchContent_Populate(std-k)
  add_subdirectory(${std-k_SOURCE_DIR} ${std-k_BINARY_DIR})
endif()

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(spacy
  GIT_REPOSITORY https://github.com/kiddersmxj/spacy
  GIT_TAG master
)
 
FetchContent_GetProperties(spacy)
if(NOT spacy_POPULATED)
  FetchContent_Populate(spacy)
  add_subdirectory(${spacy_SOURCE_DIR} ${spacy_BINARY_DIR})
endif()
 
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

# ------------------------------------------------------------------------------

project(hamza
    LANGUAGES CXX
    VERSION 0.0.0
)

if(test)
    message("Testing enabled")
    add_compile_definitions(TEST)
endif()

add_executable(hamza src/hamza.cpp)

target_sources(hamza
    PRIVATE src/hamza.cpp
    PRIVATE src/child.cpp
    PRIVATE src/assimilate.cpp
    PRIVATE src/execute.cpp
)

target_include_directories(hamza 
    PRIVATE inc
    SHARED ${std-k_SOURCE_DIR}/inc
    SHARED ${spacy_SOURCE_DIR}/inc
)

install(
    TARGETS hamza
    DESTINATION /usr/local/bin/
)

link_directories(${std-k_BINARY_DIR})
link_directories(${spacy_BINARY_DIR})
target_link_libraries(hamza
    PUBLIC std-k
    PUBLIC spacy
    PRIVATE nlohmann_json::nlohmann_json
)

# Copyright (c) 2023, Maxamilian Kidd-May
# All rights reserved.

# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree. 
