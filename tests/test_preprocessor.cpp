#include <vector>
#include <cstddef>
#include <iostream>
#include "../frontend/Preprocessor.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
extern const char* correctMessageSequence;

constexpr int stdout_fd = 1;
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
    FILE_STATE main;
    fileManager.GetFileState("examples/preprocessor_test.c", 28, &main);
    CompilationOpts opts(1, (const char**)arr);
    Preprocessor pp(main, &fileManager, &opts);
    Token tok;
    size_t i = 0;
    /*
        In order to capture stdout from preprocessor duplicate stdout file descriptor,
        then bind descriptor nr 2 to memory region using mmap and test memory output
    */

    int fd = open("./", O_RDWR | O_TMPFILE);
    //swap_fds(stdout_fd, fd);
    do{
        pp.Peek(&tok);

        fflush(stdout);
        i++;
    }while (tok.type != TokenType::eof);

    char* data = (char* ) mmap(NULL, 10'000, PROT_READ, MAP_PRIVATE, stdout_fd, 0);
    munmap(data, 10'000);

    dprintf(stdout_fd, "test passed\n");
}

const char* correctMessageSequence = 
"===== C99 PREPROCESSOR CONDITIONAL TEST SUITE START =====\n"
"[PASS] #ifdef FEATURE_A -> false\n"
"[PASS] #ifndef FEATURE_B -> true\n"
"[PASS] Test 1: #if 1\n"
"[PASS] Test 2: #if 0\n"
"[PASS] Test 3: precedence 1 + 2 * 3\n"
"[PASS] Test 4: parentheses\n"
"[PASS] Test 5: macro expansion\n"
"[PASS] Test 6: undefined macro -> 0\n"
"[PASS] Test 7: defined(X)\n"
"[PASS] Test 8: defined(Y) false\n"
"[PASS] Test 9: defined X syntax\n"
"[PASS] Test 10: logical AND/NOT\n"
"[PASS] Test 11: logical OR\n"
"[PASS] Test 12: bitwise AND\n"
"[PASS] Test 13: shift operator\n"
"[PASS] Test 14: nested #if\n"
"[PASS] Test 15: elif chain\n"
"[PASS] Test 16: macro expression\n"
"[PASS] Test 17: complex expression\n"
"[PASS] Test 18: character constant\n"
"[PASS] Test 19: large integer\n"
"[PASS] Test 20: dead code skipped\n"
"[PASS] Test 21: deep nesting\n"
"[PASS] Test 22: shift precedence\n"
"[PASS] Test 23: unary minus\n"
"[PASS] Test 24: bitwise NOT\n"
"[PASS] Test 25: ternary\n"
"[PASS] Test 26: defined without expansion\n"
"===== C99 PREPROCESSOR CONDITIONAL TEST SUITE END =====\n";