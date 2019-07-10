#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    echo 'Call : ./check_dir.sh mounted_dir'
    exit 1
fi

MOUNT_DIR=$1
ERR=5
cd $MOUNT_DIR

CMD="   Command : "

test1(){
    echo "Test 1 : Creating $1 Folders..."
    for i in $(seq 1 $1);
    do
        echo $CMD mkdir dir_$i
        mkdir dir_$i
    done
    echo "Done!"
    echo
}

create_rec(){
    if [[ $1 -le 1 ]]
    then
        return
    else
        echo $CMD mkdir dir_rec_$1
        mkdir dir_rec_$1
        
        echo $CMD cd dir_rec_$1
        cd dir_rec_$1
        
        create_rec $[$1-1]
        
        echo $CMD cd ..
        cd ..
    fi
}

iter_rec(){
    if [[ $1 -le 1 ]]
    then
        return
    else
        echo $CMD ls
        ls
        echo
        
        echo $CMD cd dir_rec_$1
        cd dir_rec_$1
        
        iter_rec $[$1-1]
        
        echo $CMD cd ..
        cd ..
    fi
}

test2(){
    echo "Test 2 : Creating $1 depth subdirectories (/a/b/c/d/e/f...)"
    create_rec $1
    echo "Done!"
    echo
}

test3(){
    echo "Test 3 : pwd, ls..."
    echo $CMD 'pwd : '
    pwd
    echo
    
    echo $CMD 'ls  : '
    ls
    echo
    
    echo $CMD 'ls -al: '
    ls -al
    echo
    
    iter_rec $1
    echo "Done!"
    echo
}

TOTAL_DIR_NUM=10
test1 $TOTAL_DIR_NUM
test2 $TOTAL_DIR_NUM
test3 $TOTAL_DIR_NUM