MODULE = bench
obj-m += $(MODULE).o
$(MODULE)-y += lifetime.o main.o
KCFLAGS := -flegacy-pass-manager -Xclang -load -Xclang $(PWD)/../../pass/build/libLifetimeChecking.so
WORKDIR := $(PWD)

all:
	make -C $(PWD)/../../linux M=$(PWD) CC=$(PWD)/../../llvm-project/build/bin/clang KCFLAGS="$(KCFLAGS)" modules

clean:
	make -C $(PWD)/../../linux M=$(PWD) CC=$(PWD)/../../llvm-project/build/bin/clang clean

IR:
	cd ../../linux && \
	$(WORKDIR)/../../llvm-project/build/bin/clang -Wp,-MMD,$(WORKDIR)/.bench.o.d -nostdinc -isystem $(WORKDIR)/../../llvm-project/build/lib/clang/14.0.1/include -I./arch/x86/include -I./arch/x86/include/generated -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -Qunused-arguments -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu89 --target=x86_64-linux-gnu -fintegrated-as -Werror=unknown-warning-option -Werror=ignored-optimization-argument -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m64 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mstack-alignment=8 -mskip-rax-setup -mtune=generic -mno-red-zone -mcmodel=kernel -Wno-sign-compare -fno-asynchronous-unwind-tables -mretpoline-external-thunk -fno-delete-null-pointer-checks -Wno-frame-address -Wno-address-of-packed-member -O2 -Wframe-larger-than=2048 -fstack-protector-strong -Wno-gnu -mno-global-merge -Wno-unused-but-set-variable -Wno-unused-const-variable -fomit-frame-pointer -ftrivial-auto-var-init=zero -enable-trivial-auto-var-init-zero-knowing-it-will-be-removed-from-clang -fno-stack-clash-protection -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wno-array-bounds -fno-strict-overflow -fno-stack-check -Werror=date-time -Werror=incompatible-pointer-types -Wno-initializer-overrides -Wno-format -Wno-sign-compare -Wno-format-zero-length -Wno-pointer-to-enum-cast -Wno-tautological-constant-out-of-range-compare -Wno-unaligned-access -fsanitize=lifetime -fsanitize=lifetime -DMODULE -DKBUILD_BASENAME=\"bench\" -DKBUILD_MODNAME=\"bench\" -D__KBUILD_MODNAME=kmod_bench -c -emit-llvm -S -o $(WORKDIR)/main.ll $(WORKDIR)/main.c