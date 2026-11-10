/*
  C interface of Stockfish.framework (StockfishFrameworkCreator)
  Copyright (C) 2021 Daniil Baranov

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

