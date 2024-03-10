// SPDX-License-Identifier: MIT

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bpf.h>
#include "pbtools.h"

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("pbtools_lkm: protobuf LKM");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define OURMODNAME   "pbtools_lkm"

static int __init pbtools_lkm_init(void)
{
    pr_info("creating kernel netlink socket\n");

    /* struct sock *
     * netlink_kernel_create(struct net *net, int unit,
     *	struct netlink_kernel_cfg *cfg)
    */
    nlsock = netlink_kernel_create(&init_net, NETLINK_MY_UNIT_PROTO, &nl_kernel_cfg);

    if (!nlsock) {
        pr_warn("netlink_kernel_create failed\n");
        return PTR_ERR(nlsock);
    }

    pr_info("inserted\n");

    return 0;		/* success */
}

static void __exit pbtools_lkm_exit(void)
{
    netlink_kernel_release(nlsock);
    pr_info("removed\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);
