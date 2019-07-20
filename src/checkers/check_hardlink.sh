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
    echo "Test 1 : link two files"
    echo
    
    FILE1="file.txt"
    FILE2="file_to_link.txt"
    CONTENT="Test for linking..."
    
    echo "$CMD echo $CONTENT > $FILE1"
    echo "$CONTENT" > $FILE1
    echo
    
    echo "$CMD ln $FILE1 $FILE2"
    ln $FILE1 $FILE2
    echo
    
    echo "$CMD ls -l"
    ls --color=auto -l
    echo
    
    echo "$CMD cat $FILE1"
    cat $FILE1
    echo
    
    echo "$CMD cat $FILE2"
    cat $FILE2
    echo
    
    echo "Done!"
    echo
}

run_test(){
    # check basic symlinking
    test1
    
}


START=$(date +%s.%N)
run_test
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)

echo "Elapsed time : $DIFF seconds"