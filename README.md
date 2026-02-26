# eBPF Snippets

> Scripting the very thing which runs your clumsy scripts

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

Then build the loader, scheduler and tc
```bash
make all
```

## Testing

### Traffic controller

```shell
sudo tc qdisc add dev <ifname> clsact
sudo tc filter add dev <ifname> ingress bpf da obj tc.bpf.o sec tc
```

Now check the generated traffic
```shell
sudo cat /sys/kernel/debug/tracing/trace_pipe
```

