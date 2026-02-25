#include "Compiler.hpp"

Compiler::Compiler(int argc, char *argv[])
:
    m_argc(argc), m_argv(argv), m_opts(argc, (const char**)argv),
    m_fileManager( m_opts.m_filenames, m_opts.m_filenameLens)
{
    
}

void Compiler::compile()
{

}
