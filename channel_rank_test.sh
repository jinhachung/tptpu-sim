#!/bin/bash
echo "Starting channel-rank test..."

rm -rf channel-rank-test
mkdir channel-rank-test
for channel in 1 2 4 8
do
    for rank in 1 2 4
    do
        outfile="./channel-rank-test/""$channel""channel""$rank""rank.output.txt"
        ./build/sim_tptpu2.exe -c $channel -r $rank -x 1600 -y 1600 -z 4800 -l nchw > $outfile
        echo Finished $outfile
    done
done

echo "Finished channel-rank test, ho ho ho~"
