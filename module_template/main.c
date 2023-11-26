// SPDX-License-Identifier: MIT

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "generated/hello_world.h"

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("pbtools_lkm_main: protobuf LKM");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

static int __init pbtools_lkm_init(void)
{

    pr_info("Start");

    int size;
    uint8_t workspace[64];
    uint8_t encoded[16];
    struct hello_world_foo_t *foo_p;

    /* Encode. */
    foo_p = hello_world_foo_new(&workspace[0], sizeof(workspace));

    if (foo_p == NULL) {
        return 1;
    }

    foo_p->bar = 78;
    size = hello_world_foo_encode(foo_p, &encoded[0], sizeof(encoded));

    if (size < 0) {
        return 2;
    }

    pr_info("Successfully encoded Foo into %d bytes.\n", size);

    /* Decode. */
    foo_p = hello_world_foo_new(&workspace[0], sizeof(workspace));

    if (foo_p == NULL) {
        return 3;
    }

    size = hello_world_foo_decode(foo_p, &encoded[0], size);

    if (size < 0) {
        return 4;
    }

    pr_info("Successfully decoded %d bytes into Foo.\n", size);
    pr_info("Foo.bar: %d\n", foo_p->bar);

	return 0;		/* success */
}

static void __exit pbtools_lkm_exit(void)
{
	pr_info("Goodbye, world\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);
