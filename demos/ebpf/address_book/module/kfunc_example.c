#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/bpf.h>

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("example_lkm: example kfunc declaration");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

// message struct to be sent
typedef struct {
    int size;
    char string[128];
} message;

/* Disables missing prototype warnings */
__bpf_kfunc_start_defs();

__bpf_kfunc message example_bpf_get_message(void)
{
    message example;
    memset(&example, 0, sizeof(message));

    example.size = 8;
    strcpy(example.string, "Custom message");

    return example;
}

__bpf_kfunc message example_bpf_print(const char *text__str)
{
    message example;
    memset(&example, 0, sizeof(message));

    example.size = 8;
    strcpy(example.string, "Custom message");

    return example;
}

__bpf_kfunc_end_defs();

static int __init pbtools_lkm_init(void)
{
    pr_info("Loaded module\n");

    return 0;
}

static void __exit pbtools_lkm_exit(void)
{
    pr_info("Removed module\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);