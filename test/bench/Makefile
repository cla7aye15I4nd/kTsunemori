obj-m+=bench.o

CFLAGS_bench.o := -fsanitize=lifetime

all:
	make -C $(PWD)/../../linux M=$(PWD) CC=$(PWD)/../../llvm-project/build/bin/clang KCFLAGS=$(CFLAGS_bench.o) modules

clean:
	make -C $(PWD)/../../linux M=$(PWD) CC=$(PWD)/../../llvm-project/build/bin/clang clean