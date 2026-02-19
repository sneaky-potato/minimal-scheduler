# Custom eBPF Scheduler

> Controlling 'HOW' your prgrams run on the CPU

---

## Getting Started

Build the required [sched/scx](https://github.com/sched-ext/scx) headers
```bash
./build_headers.sh
```

libbpf is required to build the loader, you may install that as follows
```bash
git clone https://github.com/libbpf/libbpf.git
cd libbpf/src
make
sudo make install
sudo ldconfig
```

Then build the loader and scheduler files
```bash
make all
```

