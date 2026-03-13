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
    FileManager fileManager({path}, {28});
    FILE_ID main;
    fileManager.GetFileId("examples/preprocessor_test.c", 28, &main);
    CompilationOpts opts(1, (const char**)arr);
    Parser pp(main, &fileManager, &opts); // parser is needed for complex #if/#elif directives
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


    if((size_t)st.st_size == strlen(correctMessageSequence))
    {
        for(size_t i =0; i < (size_t)st.st_size; i++)
        {
            if(correctMessageSequence[i] != data[i])
            {
                goto failed;
            }
        }
        dprintf(fd, "Preprocessor test passed\n");
        exit(1);
    }
failed:
    dprintf(fd, "Preprocessor test failed\n");
    exit(-1);
}

const char* correctMessageSequence = 
"examples/preprocessor_test.c:5:124 Preprocessor warning\n"
"===== C99 PREPROCESSOR CONDITIONAL TEST SUITE START =====\n"
"examples/preprocessor_test.c:16:467 Preprocessor warning\n"
"[PASS] #ifdef FEATURE_A -> false\n"
"examples/preprocessor_test.c:20:548 Preprocessor warning\n"
"[PASS] #ifndef FEATURE_B -> true\n"
"examples/preprocessor_test.c:33:909 Preprocessor warning\n"
"[PASS] Test 1: #if 1\n"
"examples/preprocessor_test.c:41:1050 Preprocessor warning\n"
"[PASS] Test 2: #if 0\n"
"examples/preprocessor_test.c:50:1290 Preprocessor warning\n"
"[PASS] Test 3: precedence 1 + 2 * 3\n"
"examples/preprocessor_test.c:56:1420 Preprocessor warning\n"
"[PASS] Test 4: parentheses\n"
"examples/preprocessor_test.c:70:1722 Preprocessor warning\n"
"[PASS] Test 5: macro expansion\n"
"examples/preprocessor_test.c:83:2056 Preprocessor warning\n"
"[PASS] Test 6: undefined macro -> 0\n"
"examples/preprocessor_test.c:94:2313 Preprocessor warning\n"
"[PASS] Test 7: defined(X)\n"
"examples/preprocessor_test.c:102:2468 Preprocessor warning\n"
"[PASS] Test 8: defined(Y) false\n"
"examples/preprocessor_test.c:106:2544 Preprocessor warning\n"
"[PASS] Test 9: defined X syntax\n"
"examples/preprocessor_test.c:120:2851 Preprocessor warning\n"
"[PASS] Test 10: logical AND/NOT\n"
"examples/preprocessor_test.c:126:2969 Preprocessor warning\n"
"[PASS] Test 11: logical OR\n"
"examples/preprocessor_test.c:137:3248 Preprocessor warning\n"
"[PASS] Test 12: bitwise AND\n"
"examples/preprocessor_test.c:143:3367 Preprocessor warning\n"
"[PASS] Test 13: shift operator\n"
"examples/preprocessor_test.c:158:3687 Preprocessor warning\n"
"[PASS] Test 14: nested #if\n"
"examples/preprocessor_test.c:178:4162 Preprocessor warning\n"
"[PASS] Test 15: elif chain\n"
"examples/preprocessor_test.c:193:4525 Preprocessor warning\n"
"[PASS] Test 16 macro subsitution\n"
"examples/preprocessor_test.c:205:4829 Preprocessor warning\n"
"[PASS] Test 17: complex expression\n"
"examples/preprocessor_test.c:216:5116 Preprocessor warning\n"
"[PASS] Test 18: character constant\n"
"examples/preprocessor_test.c:227:5403 Preprocessor warning\n"
"[PASS] Test 19: large integer\n"
"examples/preprocessor_test.c:246:5744 Preprocessor warning\n"
"[PASS] Test 20: dead code skipped\n"
"examples/preprocessor_test.c:258:5994 Preprocessor warning\n"
"[PASS] Test 21: deep nesting\n"
"examples/preprocessor_test.c:269:6250 Preprocessor warning\n"
"[PASS] Test 22: shift precedence\n"
"examples/preprocessor_test.c:280:6528 Preprocessor warning\n"
"[PASS] Test 23: unary minus\n"
"examples/preprocessor_test.c:286:6642 Preprocessor warning\n"
"[PASS] Test 24: bitwise NOT\n"
"examples/preprocessor_test.c:297:6926 Preprocessor warning\n"
"[PASS] Test 25: ternary\n"
"examples/preprocessor_test.c:310:7230 Preprocessor warning\n"
"[PASS] Test 26: defined without expansion\n"
"examples/preprocessor_test.c:316:7346 Preprocessor warning\n"
"===== C99 PREPROCESSOR CONDITIONAL TEST SUITE END =====\n";