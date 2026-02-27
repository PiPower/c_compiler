#include "Compiler.hpp"
#include "frontend/Parser.hpp"
Compiler::Compiler(int argc, char *argv[])
:
    argc(argc), argv(argv), opts(argc, (const char**)argv),
    fileManager( opts.filenames, opts.filenameLens)
{

}

void Compiler::compile()
{
    for(size_t i =0; i < opts.filenames.size(); i++)
    {
        FILE_STATE mainFile;
        // skip err check, constructor checks for all main files
        fileManager.GetFileState( opts.filenames[i],  opts.filenameLens[i], &mainFile);
        Parser parser(mainFile, &fileManager, &opts);
        parser.Parse();
    }
}
