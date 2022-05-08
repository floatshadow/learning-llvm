//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TokenKinds.h"
#include "llvm/Support/ErrorHandling.h"

static const char * const TokenNames[] = {
#define TOK(X) #X,
#define KEYWORD(X) #X,
#include "TokenKinds.h"
    nullptr
};

const char *tok::getTokenName(TokenKind Kind) {
  if (Kind < tok::NUM_TOKENS)
    return TokenNames[Kind];
  llvm_unreachable("unknown TokenKind");
  return nullptr;
}
