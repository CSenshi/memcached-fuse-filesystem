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
    
    echo "$CMD ln -s $FILE1 $FILE2"
    ln -s $FILE1 $FILE2
    echo
    
    echo "$CMD ls -l"
    ls -l
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


test2(){
    echo "Test 2 : link multiple files"
    echo
    
    FILE="file_mm.txt"
    FILE_MM="file_to_link_mm.txt"
    CONTENT="TEST FOR MULTIPLE"
    
    echo "$CMD echo $CONTENT > $FILE"
    echo $CONTENT > $FILE
    echo
    
    echo "$CMD ln -s $FILE $FILE_MM$((0))"
    ln -s $FILE $FILE_MM$((0))
    echo
    
    MAX_NUM=9
    for t in $(seq 0 $((MAX_NUM-1)))
    do
        echo "$CMD ln -s $FILE_MM$t $FILE_MM$((t+1))"
        ln -s $FILE_MM$t $FILE_MM$((t+1))
    done;
    
    echo
    echo "$CMD ls -l"
    ls -l
    echo
    
    echo "$CMD cat $FILE_MM$MAX_NUM"
    cat $FILE_MM$MAX_NUM
    echo
    
    echo "Done!"
    echo
}


run_test(){
    # check basic symlinking
    test1
    
    # check symlinking to multiple sources
    test2
}


START=$(date +%s.%N)
run_test
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)

echo "Elapsed time : $DIFF seconds"