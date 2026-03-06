#include "Compiler.hpp"
#include "frontend/Parser.hpp"
Compiler::Compiler(int argc, char *argv[])
:
    argc(argc), argv(argv), opts(argc, (const char**)argv),
    fileManager( opts.filenames, opts.filenameLens)
{
    // standard gcc - 11 search paths

    opts.AddSearchPath("/usr/lib/gcc/x86_64-linux-gnu/11/include", 40);
    opts.AddSearchPath("/usr/local/include", 18);
    opts.AddSearchPath("/usr/include/x86_64-linux-gnu", 29);
    opts.AddSearchPath("/usr/include", 12);

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
