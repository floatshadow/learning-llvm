

// 3 clang-AST core class: Decl, Stmt, Type
//
// int f(int a, int b) {
//  if (a>b) return a;
//  else return b;
// }
// $ clang -fsyntax-only -Xclang -ast-dump

// The topmost TranslationUnitDecl is the Root of parse tree.
// Mention that this AST has ImplicitCastExpr and marks lvalue properties
//
// TranslationUnitDecl 0x5601d27cbff8 <<invalid sloc>> <invalid sloc>
//|-TypedefDecl 0x5601d27cc8c0 <<invalid sloc>> <invalid sloc> implicit __int128_t '__int128' | `-BuiltinType 0x5601d27cc5a0 '__int128'
//|-TypedefDecl 0x5601d27cc930 <<invalid sloc>> <invalid sloc> implicit __uint128_t 'unsigned __int128' | `-BuiltinType 0x5601d27cc5c0 'unsigned __int128'
//|-TypedefDecl 0x5601d27ccc38 <<invalid sloc>> <invalid sloc> implicit __NSConstantString 'struct __NSConstantString_tag' | `-RecordType 0x5601d27cca10 'struct __NSConstantString_tag' |   `-Record 0x5601d27cc988 '__NSConstantString_tag'
//|-TypedefDecl 0x5601d27cccd0 <<invalid sloc>> <invalid sloc> implicit __builtin_ms_va_list 'char *' | `-PointerType 0x5601d27ccc90 'char *' |   `-BuiltinType 0x5601d27cc0a0 'char'
//|-TypedefDecl 0x5601d280d670 <<invalid sloc>> <invalid sloc> implicit __builtin_va_list 'struct __va_list_tag [1]' | `-ConstantArrayType 0x5601d27ccf70 'struct __va_list_tag [1]' 1 |   `-RecordType 0x5601d27ccdb0 'struct __va_list_tag' |     `-Record 0x5601d27ccd28 '__va_list_tag'
//`-FunctionDecl 0x5601d280d840 </tmp/test.c:1:1, line:6:1> line:1:5 f 'int (int, int)'
//  |-ParmVarDecl 0x5601d280d6e0 <col:7, col:11> col:11 used a 'int'
//  |-ParmVarDecl 0x5601d280d760 <col:14, col:18> col:18 used b 'int'
//  `-CompoundStmt 0x5601d280da88 <col:20, line:6:1>
//    `-IfStmt 0x5601d280da58 <line:3:4, line:5:16> has_else
//      |-BinaryOperator 0x5601d280d9a8 <line:3:8, col:10> 'int' '>'
//      | |-ImplicitCastExpr 0x5601d280d978 <col:8> 'int' <LValueToRValue>
//      | | `-DeclRefExpr 0x5601d280d938 <col:8> 'int' lvalue ParmVar 0x5601d280d6e0 'a' 'int' | `-ImplicitCastExpr 0x5601d280d990 <col:10> 'int' <LValueToRValue> |   `-DeclRefExpr 0x5601d280d958 <col:10> 'int' lvalue ParmVar 0x5601d280d760 'b' 'int'
//      |-ReturnStmt 0x5601d280da00 <line:4:8, col:15>
//      | `-ImplicitCastExpr 0x5601d280d9e8 <col:15> 'int' <LValueToRValue>
//      |   `-DeclRefExpr 0x5601d280d9c8 <col:15> 'int' lvalue ParmVar 0x5601d280d6e0 'a' 'int'
//      `-ReturnStmt 0x5601d280da48 <line:5:9, col:16>
//        `-ImplicitCastExpr 0x5601d280da30 <col:16> 'int' <LValueToRValue>
//          `-DeclRefExpr 0x5601d280da10 <col:16> 'int' lvalue ParmVar 0x5601d280d760 'b' 'int'
//

extern "C" {
#include "clang-c/Index.h"
}
#include "llvm/Support/CommandLine.h"
#include <iostream>
using namespace llvm;

static cl::opt<std::string> InputFileName(cl::Positional, cl::desc("<input file>"), cl::Required);

// visitor function
enum CXChildVisitResult // break, continue and recurse
visitASTNode(CXCursor Cursor, CXCursor Parent, CXClientData Data) {
  if (clang_getCursorKind(Cursor) == CXCursor_CXXMethod ||
      clang_getCursorKind(Cursor) == CXCursor_FunctionDecl) {
    CXString Name = clang_getCursorSpelling(Cursor);
    CXSourceLocation Location = clang_getCursorLocation(Cursor);
    std::cout << clang_getCString(Name) << std::endl;
    // Free the string
    clang_disposeString(Name);
    return CXChildVisit_Continue;
  }
  return CXChildVisit_Recurse;
}


int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv, "AST Traversal");
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
  // AST visitor cursor struct
  // Note that this cursor is not well performs in efficiency
  CXCursor Cursor = clang_getTranslationUnitCursor(TranslationUnit);
  clang_visitChildren(Cursor, visitASTNode, nullptr);
  clang_disposeTranslationUnit(TranslationUnit);
  clang_disposeIndex(Index);
  return 0;
}