CLANG ?= clang
CC ?= gcc

BPF_CFLAGS := -O2 -g -target bpf -D__TARGET_ARCH_x86
USER_CFLAGS := -O2 -g

LIBBPF_CFLAGS := $(shell pkg-config --cflags libbpf 2>/dev/null)
LIBBPF_LDLIBS := $(shell pkg-config --libs libbpf 2>/dev/null)

LIBBPF_LDLIBS ?= -lbpf

all: vmlinux.h scx_minimal.bpf.o tc.bpf.o loader

vmlinux.h:
	@echo "Generating vmlinux.h from /sys/kernel/btf/vmlinux"
	bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h

scx_minimal.bpf.o: scx_minimal.bpf.c vmlinux.h
	$(CLANG) $(BPF_CFLAGS) -I. -Iinclude -c $< -o $@

tc.bpf.o: tc.bpf.c vmlinux.h
	$(CLANG) $(BPF_CFLAGS) -I. -Iinclude -c $< -o $@

loader: loader.c
	cc -O2 -g loader.c -o loader \
	-I/usr/local/include \
	-L/usr/local/lib \
	-lbpf -lelf -lz

clean:
	rm -rf include
	rm -f *.o loader vmlinux.h

.PHONY: all clean

