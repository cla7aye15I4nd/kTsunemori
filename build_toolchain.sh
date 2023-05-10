#!/bin/bash

BASEDIR=$(dirname $(readlink -f $0))

cd $BASEDIR/llvm-project
mkdir -p build && cd build
cmake -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang" -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../llvm
make -j$(nproc)

cd $BASEDIR/pass
mkdir -p build && cd build
cmake ..
make -j$(nproc)


if [ ! -d $BASEDIR/linux ]; then
    wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.15.109.tar.xz
    tar -xvf linux-5.15.109.tar.xz
    mv linux-5.15.109 linux && cd linux
fi

cd $BASEDIR/linux
make CC=$BASEDIR/llvm-project/build/bin/clang defconfig
make CC=$BASEDIR/llvm-project/build/bin/clang -j$(nproc)