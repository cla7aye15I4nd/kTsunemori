#!/bin/bash

basedir=$(dirname $(readlink -f $0))
target=$basedir/busybox/_install

for dir in $(ls $basedir/test); do
    path=$basedir/test/$dir
    if [ -d $path ]; then
        make -C $path
        cp $path/${dir}.ko $target
        chmod 777 $target/${dir}.ko
    fi
done
