
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <bpf/libbpf.h>
#include "example.skel.h"

static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
    if (level >= LIBBPF_DEBUG)
        return 0;

    return vfprintf(stderr, format, args);
}

int main()
{
    struct example_bpf *skel;
    int err;

    // redirect libbpf errs to stdout
    libbpf_set_print(libbpf_print_fn);

    // open the .o file
    skel = example_bpf__open();

    /* space to init maps values */

    // load prog inside kernel
    example_bpf__load(skel);

    // attach prog
    err = example_bpf__attach(skel);
    if (err) {

        fprintf(stderr, "Failed to attach BPF skeleton: %d\n", err);
        goto destroy_prog;
    }

destroy_prog:
    example_bpf__destroy(skel);

    return -err;
}