#include "Compiler.hpp"
#include "frontend/Parser.hpp"
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <algorithm>
#define EXIT_ON_NONZERO(expr, msg) if(expr < 0){printf(msg); exit(-1);}

Compiler::Compiler(int argc, char *argv[])
:
    argc(argc), argv(argv), opts(), fileManager()
{
    // standard gcc - 11 search paths
    const char* SearchPaths = ".c_search_paths.set";
    FILE_ID id;
    if(fileManager.TryLoadFile(SearchPaths, strlen(SearchPaths) - 1, &id) < 0 )
    {
        ConstructPathFile();
        if(fileManager.TryLoadFile(SearchPaths, strlen(SearchPaths) - 1, &id) < 0 )
        {
            printf("FATAL ERROR: could not create \".c_search_paths.set\" file\n");
            exit(-1);
        }
    }
    // update search paths
    FILE_STATE state = {};
    fileManager.GetFileState(&id, &state);
    int64_t offset = 0;
    std::string path;
    while (offset < state.fileSize)
    {
        if(state.fileData[offset] == '\n')
        {
            opts.AddSearchPath(path.data(), path.length());
            path.clear();
        }
        else
        {
            path += state.fileData[offset];
        }
        offset++;
    }
    opts.ParseArgs(argc, (const char**)argv);
    
    //load all input file names
    assert(opts.filenames.size() == opts.filenameLens.size());
    size_t maxLen = 0;
    if(opts.filenameLens.size() > 0 )
    {
        maxLen = *std::max_element(opts.filenameLens.begin(), opts.filenameLens.end()) + 1;
    }
    char* pathBuffer = new char[maxLen];

    for(size_t i =0; i < opts.filenames.size(); i++)
    {
        memcpy(pathBuffer, opts.filenames[i], opts.filenameLens[i]);
        pathBuffer[opts.filenameLens[i]] = '\0';

        if(fileManager.TryLoadFile(pathBuffer, opts.filenameLens[i], nullptr) < 0 )
        {
            printf("File %s does not exist", pathBuffer);
            exit(-1);
        }
    }
    delete[] pathBuffer;
}

void Compiler::Compile()
{
    for(size_t i =0; i < opts.filenames.size(); i++)
    {
        FILE_ID mainFile;
        SymbolTable symtab{};
        // skip err check, constructor checks for all main files
        fileManager.GetFileId( opts.filenames[i],  opts.filenameLens[i], &mainFile);
        SemanticAnalyzer analyzer(&fileManager, &symtab);
        Parser parser(mainFile, &analyzer, &fileManager, &opts);

        while (Ast::Node* ast = parser.Parse())
        {
            analyzer.Analyze(ast);
        }
        analyzer.EmitUninitializedGlobals();
        
        char* buff = (char*)alloca(opts.filenameLens[i] + 2);
        memcpy(buff, opts.filenames[i], opts.filenameLens[i]);
        size_t j;
        for(j = opts.filenameLens[i] - 1; j > 0; j--)
        {
            if(buff[j] == '.'){break;}
        }
        buff[j + 1] = 'l';
        buff[j + 2] = 'l';
        buff[j + 3] = '\0';
        analyzer.WriteCodeToFile(buff);
    }
}

void Compiler::ConstructPathFile()
{
    const char* const echoArgs[] = {"/usr/bin/gcc", "-E", "-Wp,-v", "-", nullptr};

    int fd[2];
    EXIT_ON_NONZERO(pipe(fd), "could not create linux pipe \n");

    int ret = fork();
    EXIT_ON_NONZERO(ret, "could not fork \n");

    if(ret == 0)
    {
        // child
        int stdErr = 2;
        int stdIn = 0;
        EXIT_ON_NONZERO(close(fd[0]), "could not close read pipe end \n");
        EXIT_ON_NONZERO(close(stdErr), "could not close stdout desc \n");
        EXIT_ON_NONZERO(dup2(fd[1], stdErr), "could not duplicate pipe fd\n");
        EXIT_ON_NONZERO(close(fd[1]), "could not close write pipe end \n");
        // command gcc -E -Wp,-v -  makes gcc wait on stdin 
        // so closing it closes the program
        EXIT_ON_NONZERO(close(stdIn), "could not close stdin \n");
        EXIT_ON_NONZERO(execv(echoArgs[0], (char** const)echoArgs), "could not create child process\n");
    }
    else
    {
        // parent
        EXIT_ON_NONZERO(close(fd[1]), "FATA ERROR: could not close write pipe end \n");
    }

    FILE* pathCache = fopen(".c_search_paths.set", "w");
    // write current dir
    fwrite("./\n", 1, 3, pathCache); 

    std::string line;
    bool StartAddingPaths = false;
    // read first "
    while (true)
    {
        char c;
        ssize_t  i = read(fd[0], &c, 1);
        if(i <= 0)
        {
            break;
        }

        if(c == '\n' && line == "#include <...> search starts here:")
        {
            StartAddingPaths = true;
            line.clear();
        }
        else if(c == '\n' && line == "End of search list.")
        {
            break;
        }
        else if(c == '\n')
        {
            line += c;
            if(StartAddingPaths)
            {
                std::string_view view(line.cbegin() + 1, line.cend());
                fwrite(view.data(), 1, view.length(), pathCache);
            }
            line.clear();
        }
        else
        {
            line += c;
        }

    }
    
    fclose(pathCache);

    int wstatus;
    waitpid(ret, &wstatus, 0);

    close(fd[0]);
}
