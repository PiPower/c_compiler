import subprocess
"""
Test steps:
1: Convert c source code into sequence of tokens then reverse the process
2: Compile both files into asm
3. If result files are the same(excluding .file directive) the test is passed
"""

subprocess.run(["g++", "scannerTest.cpp", "../../src/frontend/scanner.cpp", "-o", "scannerTest"])


testFiles = [ "../functionTest.c", "../expressionTest.c", "../loopsTest.c"]
bufferFile = open("./buffer.c", "w")
passed = 0
for file in testFiles:

    content = open(file, "r+")
    p = subprocess.run(["./scannerTest"], stdin=content, stdout= bufferFile)
    bufferFile.flush()
    content.close()

    model = subprocess.run(["gcc", file, "-o", "/dev/stdout", "-S"], capture_output=True)
    reconstructed =  subprocess.run(["gcc", "./buffer.c", "-o", "/dev/stdout", "-S"], capture_output=True)
    
    model.stdout = "\n".join(model.stdout.decode("ascii").split("\n")[1:] )
    reconstructed.stdout = "\n".join(reconstructed.stdout.decode("ascii").split("\n")[1:] )

    if  model.stdout != reconstructed.stdout:
        print("FILE: " + file + " did not pass test")
    else:
        passed +=1
        
    bufferFile.truncate(0)

bufferFile.close()
print("Passed: {0} \n Total: {1}".format(passed, len(testFiles)))