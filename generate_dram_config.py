# This file looks specifically for the file named "dram-config.cfg" in the directory.
# It will replace the first line that has " speed = " in it with the the name given as argument
import sys
import os

assert len(sys.argv) == 2, "Total of 1 argument should be given"

# set variable
#program_name   = sys.argv[0]
dram_name       = sys.argv[1]

# this part is ad hoc, taken from StackOverflow: Editing specific line in text file in Python

# read file
with open("dram-config.cfg", "r") as f:
    # read a list of lines into data
    data = f.readlines()

for num in range(len(data)):
    if " speed = " in data[num]:
        data[num] = " speed = " + dram_name + "\n"
        break;

# write back (overwrite)
with open("dram-config.cfg", "w") as f:
    f.writelines(data)
