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
    echo "Test 2 : Read/Write..."
    
    TEST_FILE="TEST2_FILE.txt"
    TEST_CONTENT="1234567890-=qwertyuiop[]asdfghjklzxcvbnm,./"
    
    echo "$CMD echo $TEST_CONTENT > $TEST_FILE"
    echo $TEST_CONTENT > $TEST_FILE
    
    
    echo "$CMD cat $TEST_FILE"
    cat $TEST_FILE
    echo
}


# Base Test Make 10 files
test1 10

# Read and Write from file
test2
