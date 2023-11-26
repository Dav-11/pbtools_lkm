// SPDX-License-Identifier: MIT

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "generated/benchmark.h"

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("pbtools_lkm_main: protobuf LKM");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

// iteration param
/*static int iterations;
module_param(iterations, int, 0660);
MODULE_PARM_DESC(iterations, "Number of iterations for the test");*/

// action param
static char *action = "encode";
module_param(action, charp, 0660);
MODULE_PARM_DESC(action, "Type of test to run:\nAccepted Values:\n\t\"encode\" -> test encoding\n\t\"decode\" -> test decoding");

// debug param
/*static int mp_debug_level;
module_param(mp_debug_level, int, 0660);
MODULE_PARM_DESC(mp_debug_level, "Debug level [0-2]; 0 => no debug messages, 2 => high verbosity");*/

static void fill_message_message1(struct benchmark_message_t *message_p)
{
    benchmark_message_message1_alloc(message_p);
    message_p->message1_p->field80 = true;
    message_p->message1_p->field2 = -336;
    message_p->message1_p->field6 = 5000;
    message_p->message1_p->field22 = 5;
    benchmark_message1_field4_alloc(message_p->message1_p, 3);
    message_p->message1_p->field4.items_pp[0] = "The first string";
    message_p->message1_p->field4.items_pp[1] = "The second string";
    message_p->message1_p->field4.items_pp[2] = "The third string";
    benchmark_message1_field15_alloc(message_p->message1_p);
    message_p->message1_p->field15_p->field1 = 0;
    message_p->message1_p->field15_p->field3 = 9999;
    message_p->message1_p->field15_p->field15_p = (
            "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
            "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
            "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
            "Hello! Hello! Hello!");
    message_p->message1_p->field15_p->field12.buf_p = (uint8_t *)(
            "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
            "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
            "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
            "Hello! Hello! Hello!");
    message_p->message1_p->field15_p->field12.size = 230;
    message_p->message1_p->field15_p->field21 = 449932;
    message_p->message1_p->field15_p->field204 = 1;
    message_p->message1_p->field15_p->field300 = benchmark_e3_e;
}
static void encode_message_message1(int iterations)
{
    int i;
    struct benchmark_message_t *message_p;
    uint8_t encoded[1024];
    uint8_t workspace[1024];
    int size;

    pr_info("Encoding Message.Message1 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        message_p = benchmark_message_new(&workspace[0], sizeof(workspace));
        fill_message_message1(message_p);
        size = benchmark_message_encode(message_p, &encoded[0], sizeof(encoded));
        if (size != 566) {

            pr_crit("size is %d != 566", size);
            return;
        }
    }
}
static void decode_message_message1(int iterations)
{
    int i;
    struct benchmark_message_t *message_p;
    uint8_t encoded[1024];
    uint8_t workspace[1024];
    int size;

    message_p = benchmark_message_new(&workspace[0], sizeof(workspace));
    fill_message_message1(message_p);
    size = benchmark_message_encode(message_p, &encoded[0], sizeof(encoded));
    if (size != 566) {
        pr_crit("size is %d != 566", size);
        return;
    }

    pr_info("Decoding Message.Message1 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        message_p = benchmark_message_new(&workspace[0], sizeof(workspace));
        size = benchmark_message_decode(message_p, &encoded[0], 566);
        if (size != 566) {
            pr_crit("size is %d != 566", size);
            return;
        }
    }
}
static void fill_message3(struct benchmark_message3_t *message_p)
{
    benchmark_message3_field13_alloc(message_p, 5);

    message_p->field13.items_p[0].field28 = 7777777;
    message_p->field13.items_p[0].field2 = -3949833;
    message_p->field13.items_p[0].field12 = 1;
    message_p->field13.items_p[0].field19_p = "123";

    message_p->field13.items_p[2].field28 = 1;
    message_p->field13.items_p[2].field2 = 2;
    message_p->field13.items_p[2].field12 = 3;

    message_p->field13.items_p[3].field28 = 7777777;
    message_p->field13.items_p[3].field2 = -3949833;
    message_p->field13.items_p[3].field12 = 1;
    message_p->field13.items_p[3].field19_p = "123088410dhihf9q8hfqouwhfoquwh";

    message_p->field13.items_p[4].field28 = 4493;
    message_p->field13.items_p[4].field2 = 393211234353453ll;
}
static void encode_message3(int iterations)
{
    int i;
    struct benchmark_message3_t *message_p;
    uint8_t encoded[512];
    uint8_t workspace[512];
    int size;

    pr_info("Encoding Message3 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {

        message_p = benchmark_message3_new(&workspace[0], sizeof(workspace));
        fill_message3(message_p);
        size = benchmark_message3_encode(message_p, &encoded[0], sizeof(encoded));

        if (size != 106) {
            pr_crit("size is %d != 106", size);
        }
    }
}
static void decode_message3(int iterations)
{
    int i;
    struct benchmark_message3_t *message_p;
    uint8_t encoded[1024];
    uint8_t workspace[1024];
    int size;

    message_p = benchmark_message3_new(&workspace[0], sizeof(workspace));
    fill_message3(message_p);
    size = benchmark_message3_encode(message_p, &encoded[0], sizeof(encoded));
    if (size != 106) {
        pr_crit("size is %d != 106", size);
        return;
    }

    pr_info("Decoding Message3 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        message_p = benchmark_message3_new(&workspace[0], sizeof(workspace));
        size = benchmark_message3_decode(message_p, &encoded[0], 106);
        if (size != 106) {
            pr_crit("size is %d != 106", size);
            return;
        }
    }
}

static int __init pbtools_lkm_init(void)
{
    int mp_debug_level=1;
    int iterations=20;

    u64 start_time;

    pr_info("START");

    if (mp_debug_level > 0)
        pr_info("module parameters passed: mp_debug_level=%d action=%s\niterations=%d\n",
                mp_debug_level, action, iterations);

    start_time = ktime_get_ns();
    pr_info("startTime: %llu\n", start_time);

    if (strcmp(action, "encode") == 0) {

        encode_message_message1(iterations);
        encode_message3(iterations);
    } else {

        decode_message_message1(iterations);
        decode_message3(iterations);
    }

    pr_info("END");
	return 0;
}

static void __exit pbtools_lkm_exit(void)
{
	pr_info("Goodbye, world\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);
