// SPDX-License-Identifier: MIT

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include "generated/oneof.h"

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("pbtools_lkm_main: protobuf LKM");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");


#define OURMODNAME   "netlink_simple_intf"
#define NETLINK_MY_UNIT_PROTO   31
// kernel netlink protocol # that we're registering..

#define NLSPACE              1024

static struct sock *nlsock;

static int send_string(char *message, int pid)
{

    struct nlmsghdr *nlh;
    struct sk_buff *skb_tx;
    int msgsz, stat;

    //--- Lets be polite and reply
    msgsz = strnlen(message, NLSPACE);
    skb_tx = nlmsg_new(msgsz, 0);
    if (!skb_tx) {

        pr_warn("skb alloc failed!\n");
        return -1;
    }

    // Setup the payload
    nlh = nlmsg_put(skb_tx, 0, 0, NLMSG_DONE, msgsz, 0);
    NETLINK_CB(skb_tx).dst_group = 0;  /* unicast only (cb is the * skb's control buffer), dest group 0 => unicast */
    strncpy(nlmsg_data(nlh), message, msgsz);

    // Send it
    stat = nlmsg_unicast(nlsock, skb_tx, pid);
    if (stat < 0) {

        pr_warn("nlmsg_unicast() failed (err=%d)\n", stat);
        return -1;
    }

    return 0;
}

/*
 * netlink_recv_and_reply
 * When a userspace process (or thread) provides any input (i.e. transmits
 * something) to us, this callback function is invoked. It's important to
 * understand that it runs in process context (and not any kind of interrupt
 * context).
 */
static void netlink_recv_and_reply(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    struct oneof_foo_t *foo_p;

    char buffer[100];
    int pid, size, res;

    // arena
    uint8_t workspace[64];
    uint8_t encoded[16];

    // Receive msg
    nlh = (struct nlmsghdr *)skb->data;
    pid = nlh->nlmsg_pid;	/*pid of sending process */
    pr_info("received from PID %d:\n" "\"%s\"\n", pid, (char *)NLMSG_DATA(nlh));


    /* Encode. */
    foo_p = oneof_foo_new(&workspace[0], sizeof(workspace));

    if (foo_p == NULL) {
        pr_warn("cannot create struct\n");
        return;
    }

    oneof_foo_fie_init(foo_p);
    foo_p->fie = 789;
    size = oneof_foo_encode(foo_p, &encoded[0], sizeof(encoded));

    if (size < 0) {
        pr_warn("cannot encode struct\n");
        return;
    }

    sprintf(buffer, "Successfully encoded Foo into %d bytes.\n", size);
    res = send_string(buffer, pid);

    /* Decode. */
    foo_p = oneof_foo_new(&workspace[0], sizeof(workspace));

    if (foo_p == NULL) {
        pr_warn("cannot create struct for decode\n");
        return;
    }

    size = oneof_foo_decode(foo_p, &encoded[0], size);

    if (size < 0) {
        pr_warn("cannot decode struct\n");
        return;
    }

    sprintf(buffer, "Successfully decoded %d bytes into Foo.\n", size);
    res = send_string(buffer, pid);

    switch (foo_p->bar) {

        case oneof_foo_bar_fie_e:
            sprintf(buffer, "Foo.fie: %d\n", foo_p->fie);
            break;

        case oneof_foo_bar_fum_e:
            sprintf(buffer, "Foo.fum: %d\n", foo_p->fum);
            break;

        default:
            sprintf(buffer, "Unknown choice %d\n", foo_p->bar);
            break;
    }

    res = send_string(buffer, pid);

}

static struct netlink_kernel_cfg nl_kernel_cfg = {
    .input = netlink_recv_and_reply,
};

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