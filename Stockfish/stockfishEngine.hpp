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

#ifndef stockfishEngine_hpp
#define stockfishEngine_hpp

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

void engine_initialize();
void engine_cmd(const char *cmd);
const char *engine_getSearchMessage();
void engine_clearAllMessages();

#ifdef __cplusplus
}
#endif

#endif /* stockfishEngine_hpp */
