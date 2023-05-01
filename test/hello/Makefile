obj-m+=hello.o

all:
	make -C $(PWD)/../../linux M=$(PWD) CC=$(PWD)/../../llvm-project/build/bin/clang modules

clean:
	make -C $(PWD)/../../linux M=$(PWD) CC=$(PWD)/../../llvm-project/build/bin/clang clean