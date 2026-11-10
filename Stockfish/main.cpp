/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2026 The Stockfish developers (see AUTHORS file)

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
  Modified on 2026-11-09 for Stockfish.framework
  (StockfishFrameworkCreator); see README.md, "Changes from Stockfish 19".
  main() is replaced by functions called from stockfishEngine.cpp:
  stockfish_initialize() creates the engine unless it is already running,
  stockfish_cmd() passes one command to UCIEngine::execute(), and the engine
  output goes to a message queue (engine_message(), getSearchMessage(),
  clearAllMessages()).
*/

#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "attacks.h"
#include "misc.h"
#include "position.h"
#include "tune.h"
#include "uci.h"

using namespace Stockfish;

static std::deque<std::string>    searchMsg;
static std::mutex                 searchMsgMutex;
static std::unique_ptr<UCIEngine> uci;
static bool                       isInit = false;

void engine_message(const std::string& str) {
    std::lock_guard<std::mutex> lock(searchMsgMutex);
    searchMsg.push_back(str);
}

void clearAllMessages() {
    std::lock_guard<std::mutex> lock(searchMsgMutex);
    searchMsg.clear();
}

// The oldest message, or nullptr. The string is valid until the next call.
const char* getSearchMessage() {
    static std::string          message;
    std::lock_guard<std::mutex> lock(searchMsgMutex);
    if (searchMsg.empty())
        return nullptr;
    message = std::move(searchMsg.front());
    searchMsg.pop_front();
    return message.c_str();
}

void stockfish_initialize() {
    clearAllMessages();
    if (!isInit)
    {
        static bool tablesReady = false;
        if (!tablesReady)
        {
            set_sync_output(engine_message);
            Attacks::init();
            Position::init();
            tablesReady = true;
        }

        uci.reset();  // the engine stopped by "quit": waits for its search to finish
        uci = std::make_unique<UCIEngine>(CommandLine(0, nullptr));
        Tune::init(uci->engine_options());
        isInit = true;
    }
    std::cout << engine_info() << std::endl;
}

void stockfish_cmd(const char* cmd) {
    if (isInit && !uci->execute(cmd))
        isInit = false;  // after "quit" the engine needs stockfish_initialize() again
}
