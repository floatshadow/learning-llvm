extern "C" {
#include "clang-c/Index.h"
}
#include "llvm/Support/CommandLine.h"
#include <iostream>
using namespace llvm;

static cl::opt<std::string> InputFileName(cl::Positional, cl::desc("<input file>"), cl::Required);


// clang -Xclang --dump-tokens <input filename>
int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);
  CXIndex Index = clang_createIndex(0, 0);
  const char *args[] = {
      "-I/usr/include",
      "-I."
  };
  // TranslationUnit is a pointer to Impl struct
  CXTranslationUnit TranslationUnit =
      clang_parseTranslationUnit(
          Index,
          InputFileName.c_str(),
          args,
          2,
          nullptr,
          0,
          CXTranslationUnit_None);
  // Retrieve a file handle from translationunit
  // CXFile is a void*
  CXFile File = clang_getFile(TranslationUnit, InputFileName.c_str());
  // Set tokenize start and end position
  // We tokenize the first 60 chars
  CXSourceLocation StartLocation = clang_getLocationForOffset(TranslationUnit, File, 0);
  CXSourceLocation EndLocation   = clang_getLocationForOffset(TranslationUnit, File, 60);
  CXSourceRange  Range = clang_getRange(StartLocation, EndLocation);
  // Prepare for tokenize, declare token arrays
  unsigned num_tokens = 0;
  CXToken *Tokens = nullptr;
  clang_tokenize(TranslationUnit, Range, &Tokens, &num_tokens);
  for (unsigned int i = 0; i < num_tokens; ++i) {
    enum CXTokenKind Kind = clang_getTokenKind(Tokens[i]);
    CXString Name = clang_getTokenSpelling(TranslationUnit, Tokens[i]);
    switch (Kind) {
    case CXToken_Punctuation:
      std::cout << "Punctuation: " << clang_getCString(Name) << std::endl;
      break;
    case CXToken_Keyword:
      std::cout << "Keyword: " << clang_getCString(Name) << std::endl;
      break;
    case CXToken_Identifier:
      std::cout << "Identifier: " << clang_getCString(Name) << std::endl;
      break;
    case CXToken_Literal:
      std::cout << "Literal: " << clang_getCString(Name) << std::endl;
      break;
    case CXToken_Comment:
      std::cout << "Comment: " << clang_getCString(Name) << std::endl;
      break;
    }
    clang_disposeString(Name);
  }
  clang_disposeTokens(TranslationUnit, Tokens, num_tokens);
  clang_disposeTranslationUnit(TranslationUnit);
  return 0;
}