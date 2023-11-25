/*
 * 
 */

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("pbtools_lkm_main: protobuf LKM");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

static int __init pbtools_lkm_init(void)
{
	printk(KERN_INFO "Hello, world\n");
	return 0;		/* success */
}

static void __exit pbtools_lkm_exit(void)
{
	printk(KERN_INFO "Goodbye, world\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);
