//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LEARNING_LLVM_TOKENKINDS_H
#define LEARNING_LLVM_TOKENKINDS_H

namespace tok {

enum TokenKind : unsigned short {
#define TOK(X) X,
#include "TokenKinds.def"
  NUM_TOKENS
};

const char *getTokenName(TokenKind Kind);

};

#endif // LEARNING_LLVM_TOKENKINDS_H
