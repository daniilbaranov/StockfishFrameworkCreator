//
//  stockfishEngine.hpp
//  ChessKingLearn
//
//  Created by Daniil Baranov on 02.11.2021.
//  Copyright © 2021 Daniil Baranov. All rights reserved.
//

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
