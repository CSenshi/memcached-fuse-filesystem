#!/bin/bash

# to run please change mode
# chmod +x check_dir.bash

if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    echo 'Call : ./check_dir.sh mounted_dir'
    exit 1
fi

MOUNT_DIR=$1
cd $MOUNT_DIR


BB='1'

for b in $(seq 1 1024);
do
    KB=$KB$BB
done;

MB=''
for b in $(seq 1 1024);
do
    MB=$MB$KB
done;

write_KB(){
    echo "Test 1 : Writing 1 KB Data"
    
    START=$(date +%s.%N)
    echo $KB > "KB_Test.txt"
    END=$(date +%s.%N)
    
    DIFF=$(echo "$END - $START" | bc)
    echo "Elapsed time : $DIFF seconds"
    echo $mytime
}

write_MB(){
    echo "Test 2 : Writing 1 MB Data"
    
    START=$(date +%s.%N)
    echo $MB > "MB_Test.txt"
    END=$(date +%s.%N)
    
    DIFF=$(echo "$END - $START" | bc)
    echo "Elapsed time : $DIFF seconds"
    echo $mytime
}


write_GB(){
    echo "Test 3 : Writing 1 GB Data"
    
    START=$(date +%s.%N)
    for i in $(seq 1 1024);
    do
        if ! ((i % 10)); then
            END=$(date +%s.%N)
            DIFF=$(echo "$END - $START" | bc)
            echo "$i MB.  Elasped time : $DIFF seconds"
        fi
        echo $MB >> "GB_test.txt"
    done;
    END=$(date +%s.%N)
    
    DIFF=$(echo "$END - $START" | bc)
    echo "Elapsed time : $DIFF seconds"
    echo $mytime
}

# writes 1 KB data
write_KB

# writes 1 MB data
write_MB

# writes 1 GB data
write_GB