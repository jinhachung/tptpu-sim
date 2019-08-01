# This file looks specifically for the file named "dram-config.cfg" in the directory.
# It will replace the first line that has " speed = " in it with the the name given as argument
import sys
import os

assert len(sys.argv) == 2, "Total of 1 argument should be given"

# set variable
#program_name   = sys.argv[0]
dram_name       = sys.argv[1]

assert "DDR3" in dram_name or "DDR4" in dram_name, "DRAM type should be DDR3 or DDR4"

if ("DDR3" in dram_name):
    org = "DDR3_2Gb_x8"
else:
    org = "DDR4_4Gb_x8"

file_name = "dram-config.cfg"

# this part is ad hoc, taken from StackOverflow: Editing specific line in text file in Python

# read file
with open(file_name, "r") as f:
    # read a list of lines into data
    data = f.readlines()

# change DRAM name
for num in range(len(data)):
    if " speed = " in data[num]:
        data[num] = " speed = " + dram_name + "\n"
        break;

# change DRAM org
for num in range(len(data)):
    if " org = " in data[num]:
        data[num] = " org = " + org + "\n"
        break;

# write back (overwrite)
with open(file_name, "w") as f:
    f.writelines(data)
