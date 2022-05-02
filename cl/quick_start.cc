#include <llvm/Support/CommandLine.h>
#include <string>
#include <fstream>

using namespace llvm;

/// \brief Add parse arguments:
/// \param "o" -o arguments
/// \param cl::value_desc '-o <filename>' option
/// \param cl::desc cl option description
cl::opt<std::string> OutputFilename("o", cl::desc("Specify output filename"), cl::value_desc("filename"));

/// \brief Add a argument which first positional arguments is treated as filename
/// cl::init/
cl::opt<std::string> InputFileName(cl::Positional, cl::desc("<input file>"), cl::Required);

int main(int argc, char **argv) {
    // parse arguments and fills varibale declarations.
    cl::ParseCommandLineOptions(argc, argv);
    std::ifstream Input(InputFileName);
    std::string Text;
    Input >> Text;
    std::ofstream Output(OutputFilename);
    Output << Text;
    return 0;
}