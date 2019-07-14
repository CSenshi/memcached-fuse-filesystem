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
    echo "Test 1 : test basic chmod"
    
    echo "$CMD mkdir dir_to_chmod"
    mkdir dir_to_chmod
    
    echo "$CMD touch file_to_chmod"
    touch file_to_chmod
    
    echo "$CMD ls -l"
    ls --color=auto -l
    echo
    
    echo "$CMD chmod 0777 dir_to_chmod file_to_chmod"
    sudo chmod 0777 dir_to_chmod file_to_chmod
    echo "$CMD ls -l"
    ls --color=auto -l
    echo
    
    echo "$CMD chmod -x dir_to_chmod file_to_chmod"
    chmod -x dir_to_chmod file_to_chmod
    echo "$CMD ls -l"
    ls --color=auto -l
    echo
    
    echo "$CMD sudo chmod 0643 dir_to_chmod file_to_chmod"
    sudo chmod 0643 dir_to_chmod file_to_chmod
    echo "$CMD ls -l"
    ls --color=auto -l
    echo
    
    echo "$CMD sudo chmod 0755 file_to_chmod"
    sudo chmod 0755 file_to_chmod
    echo "$CMD sudo chmod 0644 dir_to_chmod"
    sudo chmod 0644 dir_to_chmod
    echo "$CMD ls -l"
    ls --color=auto -l
    echo
}


run_test(){
    # check basic chmod
    test1
}


START=$(date +%s.%N)
run_test
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)

echo "Elapsed time : $DIFF seconds"