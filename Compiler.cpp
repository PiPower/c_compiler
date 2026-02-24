#include "Compiler.hpp"

Compiler::Compiler(int argc, char *argv[])
:
    argc(argc), argv(argv), opts(argc, (const char**)argv),
    fileManager( opts.filenames, opts.filenameLens)
{
}

void Compiler::compile()
{

}
