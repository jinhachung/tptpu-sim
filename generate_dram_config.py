# the code should be run in the following format:
# python generate_dram_config.py dram_name num_channels num_ranks
# for example, python generate_dram_config.py DDR3_1600K 1 1

# This file looks specifically for the file named "dram-config.cfg" in the directory.
# It will replace the first line that has " standard = " in it with the appropriate standard,
# and will do the same for channels, ranks, speed, and org.
import sys
import os

assert len(sys.argv) == 4, "Total of 3 arguments should be given"

# set variable
#program_name    = sys.argv[0]
dram_name       = sys.argv[1]
channels        = sys.argv[2]
ranks           = sys.argv[3]

assert "DDR3" in dram_name or "DDR4" in dram_name, "DRAM type should be DDR3 or DDR4"

if ("DDR3" in dram_name):
    standard = "DDR3"
    org = "DDR3_2Gb_x8"
else:
    standard = "DDR4"
    org = "DDR4_4Gb_x8"

file_name = "dram-config.cfg"

# this part is ad hoc, taken from StackOverflow: Editing specific line in text file in Python

# read file
with open(file_name, "r") as f:
    # read a list of lines into data
    data = f.readlines()

#change DRAM standard
for num in range(len(data)):
    if " standard = " in data[num]:
        data[num] = " standard = " + standard + "\n"
        break;

#change number of channels
for num in range(len(data)):
    if " channels = " in data[num]:
        data[num] = " channels = " + channels + "\n"
        break;

#change number of ranks
for num in range(len(data)):
    if " ranks = " in data[num]:
        data[num] = " ranks = " + ranks + "\n"
        break;

# change DRAM name (speed)
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
