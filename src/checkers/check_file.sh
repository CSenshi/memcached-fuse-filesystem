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

CMD="#### Command : "

test1(){
    echo "Test 1 : Creating $1 Files ..."
    for i in $(seq 1 $1);
    do
        echo $CMD touch file_$i
        touch file_$i
    done
    
    echo $CMD 'ls :'
    ls
    
    echo "Done!"
    echo
}

test2(){
    echo "Test 2 : Read/Write from subdirectories..."
    
    TEST_FILE="TEST_FILE.txt"
    TEST_CONTENT="PLEASE END MY SUFFERING!!!"
    
    echo "$CMD echo $TEST_CONTENT > $TEST_FILE"
    echo $TEST_CONTENT > $TEST_FILE
    
    echo "$CMD cat $TEST_FILE"
    cat $TEST_FILE
    
    echo "Done!"
    echo
}

test3(){
    echo "Test 3 : Create files inside directories"
    
    dir1="dir1"
    dir2="$dir1/dir2"
    dir3="$dir2/dir3"
    
    echo "$CMD mkdir $dir1"
    mkdir $dir1
    echo "$CMD mkdir $dir2"
    mkdir $dir2
    echo "$CMD mkdir $dir3"
    mkdir $dir3
    
    TEST_FILE="$dir3/DONT_OPEN.txt"
    TEST_CONTENT="You Rebel!!!"
    
    echo "$CMD echo TEST_CONTENT > $TEST_FILE"
    echo $TEST_CONTENT > $TEST_FILE
    
    echo "$CMD cat $TEST_FILE"
    cat $TEST_FILE
    echo
    
    echo "Done!"
    echo
}


test4(){
    echo "Test 4 : Write/Append Data"
    
    TEST_FILE="test_append.txt"
    
    TEST_CONTENT="KILL!!!"
    echo "$CMD echo TEST_CONTENT >> $TEST_FILE"
    echo $TEST_CONTENT >> $TEST_FILE
    
    TEST_CONTENT="ME!!!"
    echo "$CMD echo TEST_CONTENT >> $TEST_FILE"
    echo $TEST_CONTENT >> $TEST_FILE
    
    TEST_CONTENT="NOW!!!"
    echo "$CMD echo TEST_CONTENT >> $TEST_FILE"
    echo $TEST_CONTENT >> $TEST_FILE
    
    TEST_CONTENT="FAST!!!"
    echo "$CMD echo TEST_CONTENT >> $TEST_FILE"
    echo $TEST_CONTENT >> $TEST_FILE
    
    echo "$CMD cat $TEST_FILE"
    cat $TEST_FILE
    echo
    
    echo "Done!"
    echo
}

run_test(){
    # Base Test Make 10 files
    test1 10
    
    # Read and Write from file
    test2
    
    # Write data inside directories
    test3
    
    # Write and append data
    test4
}


START=$(date +%s.%N)
run_test
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)

echo "Elapsed time : $DIFF seconds"