import os
import sys

if sys.argv[1]:
    os.system(f"make clean && make && ./mdriver -V -f traces/{sys.argv[1]}")
else:
    os.system(f"make clean && make && ./mdriver -V -t traces/")