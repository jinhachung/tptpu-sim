# the code should be run in the following format:
# python generate_instruction.py start_address_in_decimal jump_size tile_width tile_height total_width total_height order dirname
# for example, python generate_instruction.py 268435456 1 256 256 640 640 1 dram, where 0x10000000 = 268435456
import sys
import os

assert len(sys.argv) == 9, "Total of 8 arguments should be given"

# set variables
#program_name    = sys.argv[0]
start_address   = int(sys.argv[1])
jump_size       = int(sys.argv[2])
tile_width      = int(sys.argv[3])
tile_height     = int(sys.argv[4])
total_width     = int(sys.argv[5])
total_height    = int(sys.argv[6])
directory_name  = "build/" + sys.argv[8]
# k6_*.trc for DRAMSim2
#file_name       = directory_name + "/k6_" + sys.argv[7] + ".trc"
# *.trace for ramulator
file_name       = directory_name + "/" + sys.argv[7] + ".trace"
addrlist        = []

# make list of address
for h in range(tile_height):
    for w in range(tile_width):
        addrlist.append(start_address + jump_size * ((h * total_width) + w))

# create directory if it does not already exist, and open file in write(& create) mode
if not os.path.exists("build"):
    os.mkdir("build")
if not os.path.exists(directory_name):
    os.mkdir(directory_name)
f = open(file_name, "w+")

# convert decimal address to properly-formatted hexadecimal address
for order in range(len(addrlist)):
    # convert decimal address to hexadecimal with '0x' stripped
    hexaddr = hex(addrlist[order]).lstrip("0x")
    # add 0s at the beginning to get 00ab -> 000000ab
    while (len(hexaddr) < 8):
        hexaddr = "0" + hexaddr
    # properly format
    # 000000ab -> 0x000000AB for DRAMSim2
    #hexaddr = "0x" + hexaddr.upper()
    # 000000ab -> 0x000000ab for ramulator
    hexaddr = "0x" + hexaddr
    #write to file
    # "0x000000AB P_MEM_RD 1" for DRAMSim2
    #f.write(hexaddr + " P_MEM_RD " + str(order) + "\n")
    # "0x000000ab R" for ramulator
    f.write(hexaddr + " R\n")

f.close()
