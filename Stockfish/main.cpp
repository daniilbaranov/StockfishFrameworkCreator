/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2023 The Stockfish developers (see AUTHORS file)

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
  Modified on 2025-10-09 and 2025-10-10 for Stockfish.framework
  (StockfishFrameworkCreator); see README.md, "Changes from Stockfish 16".
  main() is replaced by functions called from stockfishEngine.cpp:
  stockfish_initialize() runs the startup code once, stockfish_cmd() passes
  one command to UCI::cmd(), and engine output goes to a message queue
  (engine_message(), getSearchMessage(), clearAllMessages()).
*/

#include <iostream>

#include "bitboard.h"
#include "endgame.h"
#include "position.h"
#include "psqt.h"
#include "search.h"
#include "syzygy/tbprobe.h"
#include "thread.h"
#include "tt.h"
#include "uci.h"

using namespace Stockfish;

static std::vector<std::string> searchMsg;
static std::mutex searchMsgVecMutex;
static bool isInit = false;

void clearAllMessages() {
  if (!searchMsg.empty()) {
    std::lock_guard<std::mutex> lock(searchMsgVecMutex);
    if (!searchMsg.empty()) {
      searchMsg.clear();
    }
  }
}

void stockfish_initialize() {
    clearAllMessages();
    if (!isInit) {
        UCI::init(Options);
        Tune::init();
        PSQT::init();
        Bitboards::init();
        Position::init();
        Bitbases::init();
        Endgames::init();
        Threads.set(size_t(Options["Threads"]));
        Search::clear(); // After threads are up
        Eval::NNUE::init();
    }
    std::cout << engine_info() << std::endl;
    isInit = true;
}

void stockfish_cmd(const char *cmd) {
    if (isInit) {
        UCI::cmd(cmd);
        if (strcmp(cmd, "quit") == 0) {
            isInit = false;
        }
    }
}

void engine_message(const std::string& str) {
  std::lock_guard<std::mutex> lock(searchMsgVecMutex);
  searchMsg.push_back(str);
}

const char* getSearchMessage() {
  if (!searchMsg.empty()) {
    std::lock_guard<std::mutex> lock(searchMsgVecMutex);
    if (!searchMsg.empty()) {
      static std::string tmpString;
      tmpString = searchMsg.front();
      searchMsg.erase(searchMsg.begin());
      return tmpString.c_str();
    }
  }
  return nullptr;
}
