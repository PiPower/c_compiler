import subprocess
import sys
import os

"""
Test steps:
1: Convert c source code into sequence of tokens then reverse the process
2: Compile both files into asm
3. If result files are the same(excluding .file directive) the test is passed
"""

subprocess.run(["g++", "./scannerTests/scannerTest.cpp", "../src/frontend/scanner.cpp", "-o", "./scannerTests/scannerTest"])


testFiles =[filename for filename in os.listdir() if filename.split(".")[-1] == 'c']
bufferFile = open("./scannerTests/buffer.c", "w")
passed = 0
for file in testFiles:
    bufferFile.truncate(0)
    bufferFile.seek(0)
    content = open(file, "r+")
    p = subprocess.run(["./scannerTests/scannerTest"], stdin=content, stdout= bufferFile)
    bufferFile.flush()
    content.close()

    model = subprocess.run(["gcc", file, "-o", "/dev/stdout", "-S"], capture_output=True)
    reconstructed =  subprocess.run(["gcc", "./scannerTests/buffer.c", "-o", "/dev/stdout", "-S"], capture_output=True)
    
    model.stdout = "\n".join(model.stdout.decode("ascii").split("\n")[1:] )
    reconstructed.stdout = "\n".join(reconstructed.stdout.decode("ascii").split("\n")[1:] )

    if  model.stdout != reconstructed.stdout:
        print("FILE: " + file + " did not pass test")
    else:
        passed +=1
        

bufferFile.close()
print("Passed: {0} \n Total: {1}".format(passed, len(testFiles)))

if passed !=  len(testFiles):
    sys.exit(-1)
