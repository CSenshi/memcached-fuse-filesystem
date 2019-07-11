#!/bin/bash

# to run please change mode
# chmod +x check_dir.bash

if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    echo 'Call : ./check_dir.sh mounted_dir'
    exit 1
fi

MOUNT_DIR=$1
ERR=5
cd $MOUNT_DIR

CMD="   Command : "

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
        echo $CMD pwd
        pwd
        echo
        
        echo $CMD cd dir_rec_$1
        cd dir_rec_$1
        
        iter_rec $[$1-1]
        
        echo $CMD cd ..
        cd ..
    fi
}

test1(){
    echo "Test 1 : Creating $1 Folders ..."
    for i in $(seq 1 $1);
    do
        echo $CMD mkdir dir_$i
        mkdir dir_$i
    done
    echo "Done!"
    echo
}

test2(){
    echo "Test 2 : Creating $1 depth subdirectories (/a/b/c/d/e/f...)"
    create_rec $1
    echo "Done!"
    echo
}

test3(){
    echo "Test 3 : pwd, ls ..."
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


test4(){
    echo "Test 3 : Creating directories with large names ..."
    
    len=128
    chars="A B C D E F G H I K L M N O P Q R S T U W Y X Z"
    for a in $chars; do
        dir_name=$a
        for i in $(seq 1 $len);
        do
            dir_name=$dir_name$a
        done
        echo mkdir "$a$a$a... ($len x)"
        mkdir $dir_name
    done;
    ls
    echo "Done!"
    echo
}

TOTAL_DIR_NUM=10

# Base Test Make 10 Directories
test1 $TOTAL_DIR_NUM

# Create 10 Subdirectories
test2 $TOTAL_DIR_NUM

# pwd and ls
test3 $TOTAL_DIR_NUM

# Create Folders with large name
test4