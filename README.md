# kTsunemori

## Installation

### Build Compiler
```bash
cd llvm-project
mkdir build && cd build
cmake -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang" -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../llvm
make -j$(nproc)
```

### Build Kernel Environment
```bash
wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.15.109.tar.xz
tar -xvf linux-5.15.109.tar.xz
mv linux-5.15.109 linux && cd linux
make CC=$(pwd)/../llvm-project/build/bin/clang defconfig
make CC=$(pwd)/../llvm-project/build/bin/clang bzImage -j$(nproc)
```

### Build Busybox
```bash
wget https://busybox.net/downloads/busybox-1.36.0.tar.bz2
tar -xvf busybox-1.36.0.tar.bz2
mv busybox-1.36.0 busybox && cd busybox
make menuconfig ## Settings > [*] Build static binary (no shared libs)
make -j$(nproc)
cd _install
mkdir -p proc sys dev etc/init.d
cat > init << EOF
#!/bin/sh
echo "INIT SCRIPT"
mkdir /tmp
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev
mount -t debugfs none /sys/kernel/debug
mount -t tmpfs none /tmp
echo -e "Boot took $(cut -d' ' -f1 /proc/uptime) seconds"
setsid /bin/cttyhack setuidgid 1000 /bin/sh
EOF
chmod +x init
find . | cpio -o --format=newc > ../rootfs.img
```
