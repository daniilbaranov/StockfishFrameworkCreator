//
//  stockfishEngine.cpp
//  ChessKingLearn
//
//  Created by Daniil Baranov on 02.11.2021.
//  Copyright © 2021 Daniil Baranov. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <vector>

#include "stockfishEngine.hpp"

void stockfish_initialize();
void stockfish_cmd(const char *cmd);
const char* getSearchMessage();
void clearAllMessages();

extern "C" const char* engine_getSearchMessage() {
    return getSearchMessage();
}

extern "C" void engine_clearAllMessages() {
    clearAllMessages();
}

extern "C" void engine_initialize() {
    stockfish_initialize();
    engine_cmd("uci\n");
}

void engine_cmd(const char *cmd) {
    stockfish_cmd(cmd);
}

