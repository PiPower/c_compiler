import subprocess
import sys
import os

"""
Test steps:
1: Convert c source code into sequence of tokens then reverse the process
2: Compile both files into asm
3. If result files are the same(excluding .file directive) the test is passed
"""

subprocess.run(["gcc", "compatibility.c", "-S"])
subprocess.run(["gcc", "../functionTest.c", "compatibility.s", "-o", "model"])
subprocess.run(["../../build/c_compiler", "../functionTest.c"])
subprocess.run(["gcc", "test.s", "compatibility.s", "-o", "validator"])

model = subprocess.run(["./model"], capture_output=True)
validator = subprocess.run(["./validator"], capture_output=True)

os.remove("compatibility.s")
os.remove("model")
os.remove("validator")
os.remove("test.s")

if model.stdout.decode("ascii") != validator.stdout.decode("ascii"):
    print(model.stdout.decode("ascii"))
    print("---------")
    print(validator.stdout.decode("ascii"))
    sys.exit(-1)