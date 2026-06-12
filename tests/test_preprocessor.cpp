#include <vector>
#include <cstddef>
#include <iostream>
#include "../frontend/Parser.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
extern const char* correctMessageSequence;

int swap_fds(int fd1, int fd2)
{
    int tmp = dup(fd1);
    if (tmp < 0)
        return -1;

    if (dup2(fd2, fd1) < 0)
        return -1;

    if (dup2(tmp, fd2) < 0)
        return -1;

    close(tmp);
    return 0;
}

int main()
{
    const char* path = "examples/preprocessor_test.c";
    const char* arr[] = {path};
    FileManager fileManager;
    fileManager.TryLoadFile(path, 28, nullptr);
    FILE_ID main;
    fileManager.GetFileId("examples/preprocessor_test.c", 28, &main);
    CompilationOpts opts;
    opts.ParseArgs(1, (const char**)arr);
    SymbolTable symtab;
    SemanticAnalyzer analyzer(&fileManager, &symtab);
    Parser pp(main,&analyzer, &fileManager, &opts); // parser is needed for complex #if/#elif directives
    /*
        In order to capture stdout from preprocessor duplicate stdout file descriptor,
        then bind descriptor nr 2 to memory region using mmap and test memory output
    */
    int stdout_fd = 1;
    int fd = open("./", O_RDWR | O_TMPFILE);
    swap_fds(stdout_fd, fd);
    pp.Parse();
    fflush(stdout);

    char* data = (char* ) mmap(NULL, 10'000, PROT_READ, MAP_PRIVATE, stdout_fd, 0);
    if (data == MAP_FAILED)
    {
        // mmap failed, print the error
        perror("mmap failed");
        close(fd);
        return 1;
    }
    
    struct stat st;
    if (fstat(stdout_fd, &st) == -1) 
    {
        perror("fstat failed");
        close(fd);
        return 1;
    }

    size_t minLen = std::min((size_t)st.st_size, strlen(correctMessageSequence));

    for(size_t i =0; i < minLen; i++)
    {
        if(correctMessageSequence[i] != data[i])
        {
            goto failed;
        }
    }
    dprintf(fd, "Preprocessor test passed\n");
    exit(1);
    
failed:
    dprintf(fd, "Preprocessor test failed\n");
    exit(-1);
}

const char* correctMessageSequence2 = 
    "examples/preprocessor_test.c:5:124\n"
    "Preprocessor warning:\n";

const char* correctMessageSequence = 
    "examples/preprocessor_test.c:4:123\n"
    "Preprocessor warning:\n"
    "===== C99 PREPROCESSOR CONDITIONAL TEST SUITE START =====\n"
    "examples/preprocessor_test.c:15:466\n"
    "Preprocessor warning:\n"
    "[PASS] #ifdef FEATURE_A -> false\n"
    "examples/preprocessor_test.c:19:547\n"
    "Preprocessor warning:\n"
    "[PASS] #ifndef FEATURE_B -> true\n"
    "examples/preprocessor_test.c:32:908\n"
    "Preprocessor warning:\n"
    "[PASS] Test 1: #if 1\n"
    "examples/preprocessor_test.c:40:1049\n"
    "Preprocessor warning:\n"
    "[PASS] Test 2: #if 0\n"
    "examples/preprocessor_test.c:49:1289\n"
    "Preprocessor warning:\n"
    "[PASS] Test 3: precedence 1 + 2 * 3\n"
    "examples/preprocessor_test.c:55:1419\n"
    "Preprocessor warning:\n"
    "[PASS] Test 4: parentheses\n"
    "examples/preprocessor_test.c:69:1721\n"
    "Preprocessor warning:\n"
    "[PASS] Test 5: macro expansion\n"
    "examples/preprocessor_test.c:82:2055\n"
    "Preprocessor warning:\n"
    "[PASS] Test 6: undefined macro -> 0\n"
    "examples/preprocessor_test.c:93:2312\n"
    "Preprocessor warning:\n"
    "[PASS] Test 7: defined(X)\n"
    "examples/preprocessor_test.c:101:2467\n"
    "Preprocessor warning:\n"
    "[PASS] Test 8: defined(Y) false\n"
    "examples/preprocessor_test.c:105:2543\n"
    "Preprocessor warning:\n"
    "[PASS] Test 9: defined X syntax\n"
    "examples/preprocessor_test.c:119:2850\n"
    "Preprocessor warning:\n"
    "[PASS] Test 10: logical AND/NOT\n"
    "examples/preprocessor_test.c:125:2968\n"
    "Preprocessor warning:\n"
    "[PASS] Test 11: logical OR\n"
    "examples/preprocessor_test.c:136:3247\n"
    "Preprocessor warning:\n"
    "[PASS] Test 12: bitwise AND\n"
    "examples/preprocessor_test.c:142:3366\n"
    "Preprocessor warning:\n"
    "[PASS] Test 13: shift operator\n"
    "examples/preprocessor_test.c:157:3686\n"
    "Preprocessor warning:\n"
    "[PASS] Test 14: nested #if\n"
    "examples/preprocessor_test.c:177:4161\n"
    "Preprocessor warning:\n"
    "[PASS] Test 15: elif chain\n"
    "examples/preprocessor_test.c:192:4524\n"
    "Preprocessor warning:\n"
    "[PASS] Test 16 macro subsitution\n"
    "examples/preprocessor_test.c:204:4828\n"
    "Preprocessor warning:\n"
    "[PASS] Test 17: complex expression\n"
    "examples/preprocessor_test.c:215:5115\n"
    "Preprocessor warning:\n"
    "[PASS] Test 18: character constant\n"
    "examples/preprocessor_test.c:226:5402\n"
    "Preprocessor warning:\n"
    "[PASS] Test 19: large integer\n"
    "examples/preprocessor_test.c:245:5743\n"
    "Preprocessor warning:\n"
    "[PASS] Test 20: dead code skipped\n"
    "examples/preprocessor_test.c:257:5993\n"
    "Preprocessor warning:\n"
    "[PASS] Test 21: deep nesting\n"
    "examples/preprocessor_test.c:268:6249\n"
    "Preprocessor warning:\n"
    "[PASS] Test 22: shift precedence\n"
    "examples/preprocessor_test.c:279:6527\n"
    "Preprocessor warning:\n"
    "[PASS] Test 23: unary minus\n"
    "examples/preprocessor_test.c:285:6641\n"
    "Preprocessor warning:\n"
    "[PASS] Test 24: bitwise NOT\n"
    "examples/preprocessor_test.c:296:6925\n"
    "Preprocessor warning:\n"
    "[PASS] Test 25: ternary\n"
    "examples/preprocessor_test.c:309:7229\n"
    "Preprocessor warning:\n"
    "[PASS] Test 26: defined without expansion\n"
    "examples/preprocessor_test.c:315:7345\n"
    "Preprocessor warning:\n"
    "===== C99 PREPROCESSOR CONDITIONAL TEST SUITE END =====\n";