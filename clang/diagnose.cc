extern "C" {
// public interface to Clang library 
// without exposing full Clang C++ API
#include <clang-c/Index.h>
}
#include <llvm/Support/CommandLine.h>
#include <fstream>
#include <iostream>
#include <string>

using namespace llvm;

static cl::opt<std::string> InputFileName(cl::Positional, cl::desc("<input file>"), cl::Required);


int main(int argc, char **argv) {
    cl::ParseCommandLineOptions(argc, argv, "Clang Diagnose");
    // It receives two integer-encoded Booleans as parameters: 
    // the first is true if we want to exclude declarations from precompiled headers (PCH) 
    // and the second is true if we want to display diagnostics. We set
    // both to false (zero) because we want to display the diagnostics by ourselves.
    CXIndex Index = clang_createIndex(0, 0);

    const char *command_line_args[] = {
        "-I/usr/include",
        "-I."
    };
    // Translation unit manipulation
    // https://clang.llvm.org/doxygen/group__CINDEX__TRANSLATION__UNIT.html
    CXTranslationUnit TranslationUnit = clang_parseTranslationUnit(
                                        Index,
                                        InputFileName.c_str(),
                                        command_line_args,
                                        2, // num of command line args
                                        NULL, // unsaved files
                                        0, // num of unsaved files
                                        CXTranslationUnit_None); // no special translation unit option

    // get Clang diagnose
    unsigned DiagnosticCount = clang_getNumDiagnostics(TranslationUnit);
    for (unsigned i = 0; i < DiagnosticCount; ++i) {
        CXDiagnostic Diagnostic = clang_getDiagnostic(TranslationUnit, i);
        // parse error categories, like "Semantic Issue", "Parse Issue"
        CXString Category = clang_getDiagnosticCategoryText(Diagnostic);
        // error cause
        CXString Message = clang_getDiagnosticSpelling(Diagnostic);
        // severity level 
        unsigned Severity = clang_getDiagnosticSeverity(Diagnostic);
        // error location: file, row, col tuple
        CXSourceLocation Location = clang_getDiagnosticLocation(Diagnostic);
        CXString FileName;
        unsigned line = 0, col = 0;
        clang_getPresumedLocation(Location, &FileName, &line, &col);
        // print diagnose infomation
        std::cout << "Severity: " << Severity 
                  << " File: " << clang_getCString(FileName) 
                  << " Line: " << line
                  << " Col: " << col
                  << " Category: " << clang_getCString(Category)
                  << " Message: " << clang_getCString(Message) << std::endl;
        // Free CXStrings
        clang_disposeString(FileName);
        clang_disposeString(Category);
        clang_disposeString(Message);
        clang_disposeDiagnostic(Diagnostic);   
    }
    clang_disposeTranslationUnit(TranslationUnit);
    clang_disposeIndex(Index);
    return 0;
}
