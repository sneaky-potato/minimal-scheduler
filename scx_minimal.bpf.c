#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <scx/common.bpf.h>

char LICENSE[] SEC("license") = "GPL";

/*
 * select_cpu:
 * Return the previous CPU
 */
s32 BPF_STRUCT_OPS(minimal_select_cpu, struct task_struct *p, s32 prev_cpu, u64 wake_flags)
{
    return prev_cpu;
}

/*
 * enqueue:
 * Even PID  -> local DSQ
 * Odd PID   -> global DSQ
 */
void BPF_STRUCT_OPS(minimal_enqueue, struct task_struct *p, u64 enq_flags)
{
    if (p->pid % 2 == 0) {
        /* Dispatch directly to the CPU-local DSQ */
        scx_bpf_dsq_insert(p, SCX_DSQ_LOCAL, SCX_SLICE_DFL, 0);
    } else {
        /* Dispatch to global DSQ */
        scx_bpf_dsq_insert(p, SCX_DSQ_GLOBAL, SCX_SLICE_DFL, 0);
    }
}

/*
 * dispatch:
 * First try local (automatic),
 * then consume from global if needed.
 */
void BPF_STRUCT_OPS(minimal_dispatch, s32 cpu, struct task_struct *prev)
{
    /*
     * If local DSQ empty, pull from global.
     */
    scx_bpf_dsq_move_to_local(SCX_DSQ_GLOBAL);
}

/*
 * init & exit 
 */
s32 BPF_STRUCT_OPS(minimal_init)
{
    return 0;
}

void BPF_STRUCT_OPS(minimal_exit, struct scx_exit_info *ei)
{
}

/*
 * Register scheduler
 */
SEC(".struct_ops")
struct sched_ext_ops minimal_ops = {
    .select_cpu    = (void *)minimal_select_cpu,
    .enqueue       = (void *)minimal_enqueue,
    .dispatch      = (void *)minimal_dispatch,
    .init          = (void *)minimal_init,
    .exit          = (void *)minimal_exit,
    .name          = "scx_minimal",
};

