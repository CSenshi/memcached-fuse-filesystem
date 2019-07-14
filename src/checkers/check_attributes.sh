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
    echo "Test 1 : set/get/remove/list External Attributes"
    
    FILE_NAME="test_attr_1.txt"
    
    echo "$CMD touch $FILE_NAME"
    touch $FILE_NAME
    echo
    
    ATTR_KEY="TEST_KEY"
    ATTR_VAL="TEST_VALUE"
    
    # set atribute
    echo "$CMD setfattr -n user.$ATTR_KEY -v $ATTR_VAL $FILE_NAME"
    setfattr -n user.$ATTR_KEY -v $ATTR_VAL $FILE_NAME
    echo
    
    ATTR_KEY2="TEST_KEY2"
    ATTR_VAL2="TEST_VALUE2"
    
    # set atribute
    echo "$CMD setfattr -n user.$ATTR_KEY2 -v $ATTR_VAL2 $FILE_NAME"
    setfattr -n user.$ATTR_KEY2 -v $ATTR_VAL2 $FILE_NAME
    echo
    
    # get atribute
    echo "$CMD getfattr -n user.$ATTR_KEY $FILE_NAME (should return value)"
    getfattr -n user.$ATTR_KEY $FILE_NAME
    
    # list atributes
    echo "$CMD getfattr -d $FILE_NAME (should return full list of key:value)"
    getfattr -d $FILE_NAME
    
    # remove atribute
    echo "$CMD setfattr -x user.$ATTR_KEY $FILE_NAME"
    setfattr -x user.$ATTR_KEY $FILE_NAME
    echo
    
    # get atribute
    echo "$CMD getfattr -n user.$ATTR_KEY $FILE_NAME (should not return)"
    getfattr -n user.$ATTR_KEY $FILE_NAME
    echo
    
    echo "Done!"
    echo
}

test2(){
    echo "Test 2 : set/get/remove External Attributes"
    
    FILE_NAME="test_attr_2.txt"
    
    
    echo "$CMD touch $FILE_NAME"
    touch $FILE_NAME
    echo
    
    ATTR_KEY="TEST_KEY"
    ATTR_VAL="TEST_VALUE"
    
    TOTAL=50
    
    # set atribute
    for t in $(seq 1 $TOTAL);
    do
        setfattr -n user.$ATTR_KEY$t -v $ATTR_VAL$t $FILE_NAME
    done;
    
    # list atributes
    echo "$CMD getfattr -d $FILE_NAME (should return full list of key:value)"
    getfattr -d $FILE_NAME
    
    echo "Removing all of them"
    
    # remove atributes
    for t in $(seq 1 $TOTAL);
    do
        # echo user.$ATTR_KEY$t
        setfattr -x user.$ATTR_KEY$t $FILE_NAME
    done;
    
    # list atributes
    echo "$CMD getfattr -d $FILE_NAME (should not return list)"
    getfattr -d $FILE_NAME
    
    echo "Done!"
    echo
}

test3(){
    echo "Test 3 : set/get/remove/list External Attributes"
    
    FILE_NAME="test_attr_3.txt"
    
    echo "$CMD touch $FILE_NAME"
    touch $FILE_NAME
    echo
    
    ATTR_KEY="TEST_KEY"
    ATTR_VAL="TEST_VALUE"
    
    # set atribute
    echo "$CMD setfattr -n user.$ATTR_KEY -v $ATTR_VAL $FILE_NAME"
    setfattr -n user.$ATTR_KEY -v $ATTR_VAL $FILE_NAME
    echo
    
    # get atribute
    echo "$CMD getfattr -n user.$ATTR_KEY $FILE_NAME (should return value)"
    getfattr -n user.$ATTR_KEY $FILE_NAME
    
    # set atribute
    ATTR_VAL="TEST_VALUE_REPL_1"
    echo "$CMD setfattr -n user.$ATTR_KEY -v $ATTR_VAL $FILE_NAME"
    setfattr -n user.$ATTR_KEY -v $ATTR_VAL $FILE_NAME
    echo
    
    # get atribute
    echo "$CMD getfattr -n user.$ATTR_KEY $FILE_NAME (should return value)"
    getfattr -n user.$ATTR_KEY $FILE_NAME
    
    # set atribute
    ATTR_VAL="TEST_VALUE_REPL_2"
    echo "$CMD setfattr -n user.$ATTR_KEY -v $ATTR_VAL $FILE_NAME"
    setfattr -n user.$ATTR_KEY -v $ATTR_VAL $FILE_NAME
    echo
    
    # get atribute
    echo "$CMD getfattr -n user.$ATTR_KEY $FILE_NAME (should return value)"
    getfattr -n user.$ATTR_KEY $FILE_NAME
    
}

run_test(){
    # check basic set and get attributes
    test1
    
    # check for multiple attributes
    test2
    
    # check for replacement
    test3
}


START=$(date +%s.%N)
run_test
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)

echo "Elapsed time : $DIFF seconds"