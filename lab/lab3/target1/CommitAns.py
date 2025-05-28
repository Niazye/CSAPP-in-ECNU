#!/usr/bin/env python3
# coding=utf-8

import os
import sys

def gene(file, level):
    cmd = f"./hex2raw < Solution_files/{file}target_lv{level}/ans_hex.txt > Solution_files/{file}target_lv{level}/ans.in"
    print(f"\033[1;31mRun: \t{cmd}\033[0m")
    out = os.popen(cmd).read()
    if out:
        print("\033[1;35mvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\033[0m")
        print(out)
        print("\033[1;35m^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\033[0m")
    else:
        print("\033[1;35mDone\033[0m")
def chck(file, level):
    cmd = f"./{file}target -q -i Solution_files/{file}target_lv{level}/ans.in"
    print(f"\033[1;31mRun: \t{cmd}\033[0m")
    out = os.popen(cmd).read()
    if out:
        print("\033[1;35mvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\033[0m")
        print(out)
        print("\033[1;35m^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\033[0m")
    else:
        print("\033[1;35mDone\033[0m")
file, level = sys.argv[1], sys.argv[2]
if file == 'a':
    gene('c', 1)
    gene('c', 2)
    gene('c', 3)
    gene('r', 1)
    gene('r', 2)
    chck('c', 1)
    chck('c', 2)
    chck('c', 3)
    chck('r', 1)
    chck('r', 2)
elif level == '0':
    if file == 'c':
        gene('c', 1)
        gene('c', 2)
        gene('c', 3)
        chck('c', 1)
        chck('c', 2)
        chck('c', 3)
    elif file == 'r':
        gene('r', 1)
        gene('r', 2)
        chck('r', 1)
        chck('r', 2)
else:
    gene(file, level)
    chck(file, level)
