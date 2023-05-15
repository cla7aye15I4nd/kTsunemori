MODULE = baseline
obj-m += $(MODULE).o
$(MODULE)-y += main.o
WORKDIR := $(PWD)

all:
	make -C $(PWD)/../../linux M=$(PWD) CC=$(PWD)/../../llvm-project/build/bin/clang modules

clean:
	make -C $(PWD)/../../linux M=$(PWD) CC=$(PWD)/../../llvm-project/build/bin/clang clean
