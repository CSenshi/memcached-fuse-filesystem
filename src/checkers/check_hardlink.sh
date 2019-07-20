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

test2(){
    echo "Test 2 : link two files inside same directory"
    
    mkdir test2_dir
    cd test2_dir
    mkdir dir
    cd dir
    
    FILE1="file.txt"
    FILE2="file_to_link.txt"
    CONTENT="Test for linking inside directories..."
    
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
    
    cd ..
    cd ..
    
    echo "Done!"
    echo
}

test3(){
    echo "Test 3 : link two files inside different directory"
    
    mkdir test3_dir
    cd test3_dir
    mkdir dir1
    mkdir dir1/dir2
    
    FILE1="dir1/dir2/file.txt"
    FILE2="file_to_link.txt"
    CONTENT="Test for linking inside directories..."
    
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
    
    cd ..
    echo "Done!"
    echo
}

test4(){
    echo "Test 4 : link multiple files"
    echo
    
    FILE="file_mm.txt"
    FILE_MM="file_to_link_mm.txt"
    CONTENT="TEST FOR MULTIPLE"
    
    echo "$CMD echo $CONTENT > $FILE"
    echo $CONTENT > $FILE
    echo
    
    echo "$CMD ln $FILE $FILE_MM$((0))"
    ln $FILE $FILE_MM$((0))
    echo
    
    MAX_NUM=9
    for t in $(seq 0 $((MAX_NUM-1)))
    do
        echo "$CMD ln $FILE_MM$t $FILE_MM$((t+1))"
        ln $FILE_MM$t $FILE_MM$((t+1))
    done;
    
    echo
    echo "$CMD ls -l"
    ls --color=auto -l
    echo
    
    echo "$CMD cat $FILE_MM$MAX_NUM"
    cat $FILE_MM$MAX_NUM
    echo
    
    echo "Done!"
    echo
}

test5(){
    echo "Test 5 : link direcotires"
    
    DIR="dir"
    DIR_TO_LINK="dir_to_link"
    
    mkdir $DIR
    mkdir $DIR/a
    mkdir $DIR/b
    mkdir $DIR/c
    mkdir $DIR/d
    touch $DIR/file
    
    echo "$CMD ln $DIR $DIR_TO_LINK"
    ln $DIR $DIR_TO_LINK
    echo
    
    echo "$CMD ls -l"
    ls --color=auto -l
    echo
    
    echo "$CMD ls $DIR"
    ls --color=auto  $DIR
    echo
    
    echo "$CMD ls  $DIR_TO_LINK"
    ls --color=auto  $DIR_TO_LINK
    echo
    
}

test5(){
    echo "Test 5 : link three files and delete mid one"
    echo
    
    FILE1="file.txt"
    FILE2="file1.txt"
    FILE3="file2.txt"
    CONTENT="Test for linking..."
    
    echo "$CMD echo $CONTENT > $FILE1"
    echo "$CONTENT" > $FILE1
    echo
    
    echo "$CMD ln $FILE1 $FILE2"
    ln $FILE1 $FILE2
    echo
    
    echo "$CMD ln $FILE2 $FILE3"
    ln $FILE2 $FILE3
    echo
    
    echo "$CMD ls -l"
    ls --color=auto -l
    echo
    
    echo "$CMD cat $FILE1"
    cat $FILE1
    echo
    
    echo "$CMD cat $FILE3"
    cat $FILE2
    echo
    
    echo "$CMD rm $FILE2"
    rm $FILE2
    echo
    
    echo "$CMD ls -l"
    ls --color=auto -l
    echo
    
    echo "$CMD cat $FILE3"
    cat $FILE3
    echo
    
    echo "Done!"
    echo
    
}


run_test(){
    # check basic hardlinking
    # test1
    
    # # hardlinking inside same directories
    # test2
    
    # # hardlinking inside different directories
    # test3
    
    # # check hardlinking to multiple sources
    # test4
    
    # check for deletin sources in hardlinking
    test5
}


START=$(date +%s.%N)
run_test
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)

echo "Elapsed time : $DIFF seconds"