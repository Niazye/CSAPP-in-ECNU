#!/usr/bin/env python3

import os
import sys

file, level = sys.argv[1], sys.argv[2]
commands = [f"./hex2raw < Solution_files/{file}target_lv{level}/ans_hex.txt > Solution_files/{file}target_lv{level}/ans.in",
            f"./{file}target -q -i Solution_files/{file}target_lv{level}/ans.in"]
print(f"\033[1;31mRun: \t{commands[0]}\033[0m")
out = os.popen(commands[0]).read()
if out:
    print("\033[1;35mvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\033[0m")
    print(out)
    print("\033[1;35m^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\033[0m\n")
else:
    print("\033[1;35mDone\033[0m\n")
print(f"\033[1;31mRun: \t{commands[1]}\033[0m")
out = os.popen(commands[1]).read()
if out:
    print("\033[1;35mvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\033[0m")
    print(out)
    print("\033[1;35m^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\033[0m\n")
else:
    print("\033[1;35mDone\033[0m\n")
