# Run the benchmark

The source repository had a [folder](https://github.com/eerimoq/pbtools/tree/master/benchmark) with benchmarks of various libraries.
The idea is to replicate the same test with our in-kernel implementation.

## Try 1
- Created a subfolder inside the benchmark folder with a copy of the `module_template` folder.
- Compiled the `benchmark.proto`
- Edited the `main.c` file based on the original pbtools [benchmark](https://github.com/eerimoq/pbtools/blob/master/benchmark/pbtools/main.c):
```c
static int __init pbtools_lkm_init(void)
{
    int mp_debug_level=1;
    int iterations=20;

    u64 start_time;

    pr_info("START");

    if (mp_debug_level > 0)
        pr_info("module parameters passed: mp_debug_level=%d action=%s\niterations=%d\n",
                mp_debug_level, action, iterations);

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
```

### Problem
Sometimes the logs do not show the output of the `pr_info("END");`.