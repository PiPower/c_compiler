#include "Compiler.hpp"
#include "frontend/Parser.hpp"
Compiler::Compiler(int argc, char *argv[])
:
    m_argc(argc), m_argv(argv), m_opts(argc, (const char**)argv),
    m_fileManager( m_opts.m_filenames, m_opts.m_filenameLens)
{

}

void Compiler::compile()
{
    for(size_t i =0; i < m_opts.m_filenames.size(); i++)
    {
        FILE_STATE mainFile;
        // skip err check, constructor checks for all main files
        m_fileManager.GetFileState( m_opts.m_filenames[i],  m_opts.m_filenameLens[i], &mainFile);
        Parser parser(mainFile, &m_fileManager, &m_opts);
        parser.Parse();
    }
}
