#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <bpf/libbpf.h>

static volatile sig_atomic_t exiting = 0;

static void sig_handler(int signo)
{
    exiting = 1;
}

int main(int argc, char **argv)
{
    struct bpf_object *obj;
    struct bpf_link *link = NULL;
    struct bpf_map *map;
    int err = 0;

    libbpf_set_strict_mode(LIBBPF_STRICT_ALL);
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    obj = bpf_object__open_file("scx_minimal.bpf.o", NULL);
    if (libbpf_get_error(obj)) {
        fprintf(stderr, "Failed to open BPF object\n");
        return 1;
    }

    err = bpf_object__load(obj);
    if (err) {
        fprintf(stderr, "Failed to load BPF object: %d\n", err);
        goto cleanup;
    }

    /* Find the struct_ops map (defined with SEC(".struct_ops") in BPF prog) */
    map = bpf_object__find_map_by_name(obj, "minimal_ops");
    if (!map) {
        fprintf(stderr, "Failed to find struct_ops map 'minimal_ops'\n");
        err = -1;
        goto cleanup;
    }

    link = bpf_map__attach_struct_ops(map);
    if (libbpf_get_error(link)) {
        fprintf(stderr, "Failed to attach struct_ops\n");
        link = NULL;
        err = -1;
        goto cleanup;
    }

    printf("sched_ext minimal scheduler loaded.\n");
    printf("Press Ctrl+C to exit.\n");

    while (!exiting) {
        sleep(1);
    }

cleanup:
    if (link)
        bpf_link__destroy(link);
    if (obj)
        bpf_object__close(obj);
    printf("Exiting.\n");
    return err != 0;
}
