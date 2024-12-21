import subprocess
import sys
import os


subprocess.run(["g++", "expr_test.cpp", "-o", "expr_test", "-I ../../frontend", "../../build/libfrontend.a"])
true_result = """(23 + 54)
((((68 * 3) * ((293 + 32) - 4)) / x) + y)
((((c * 2) + (23 * (2 = g))) = (23 = b)) = x)\n"""

stdoutBuff = ""
passed = 0

p = subprocess.run(["./expr_test", "../../examples/expr.sael"], capture_output = True)
os.remove("expr_test")
if str.encode(true_result, encoding="ascii") == p.stdout:
    sys.exit(0)
else:
    sys.exit(-1)