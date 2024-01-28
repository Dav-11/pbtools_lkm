// SPDX-License-Identifier: MIT

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include "generated/address_book.h"

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("pbtools_lkm_main: protobuf LKM");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");


#define OURMODNAME   "netlink_simple_intf"
#define NETLINK_MY_UNIT_PROTO   31
// kernel netlink protocol # that we're registering..

#define NLSPACE              1024

static struct sock *nlsock;

static int encode_decode(void)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[1024];
    struct address_book_address_book_t *address_book_p;
    struct address_book_person_t *person_p;
    struct address_book_person_phone_number_t *phone_number_p;

    address_book_p = address_book_address_book_new(&workspace[0], sizeof(workspace));

    /* Add one person to the address book. */
    WARN_ON(address_book_address_book_people_alloc(address_book_p, 1) != 0);
    person_p = &address_book_p->people.items_p[0];
    person_p->name_p = "Kalle Kula";
    person_p->id = 56;
    person_p->email_p = "kalle.kula@foobar.com";

    /* Add phone numbers. */
    WARN_ON(address_book_person_phones_alloc(person_p, 2) != 0);

    /* Home. */
    phone_number_p = &person_p->phones.items_p[0];
    phone_number_p->number_p = "+46701232345";
    phone_number_p->type = address_book_person_home_e;

    /* Work. */
    phone_number_p = &person_p->phones.items_p[1];
    phone_number_p->number_p = "+46999999999";
    phone_number_p->type = address_book_person_work_e;

    /* Encode the message. */
    size = address_book_address_book_encode(address_book_p,
                                            &encoded[0],
                                            sizeof(encoded));
    WARN_ON(size != 75);

    /* Decode the message. */
    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    WARN_ON(address_book_p == NULL);
    size = address_book_address_book_decode(address_book_p, &encoded[0], size);
    WARN_ON(size < 0);
    WARN_ON(address_book_p->people.length != 1);

    /* Check the decoded person. */
    person_p = &address_book_p->people.items_p[0];
    WARN_ON(strcmp(person_p->name_p, "Kalle Kula") != 0);
    WARN_ON(person_p->id != 56);
    WARN_ON(strcmp(person_p->email_p, "kalle.kula@foobar.com") != 0);
    WARN_ON(person_p->phones.length != 2);

    /* Check home phone number. */
    phone_number_p = &person_p->phones.items_p[0];
    WARN_ON(strcmp(phone_number_p->number_p, "+46701232345") != 0);
    WARN_ON(phone_number_p->type != address_book_person_home_e);

    /* Check work phone number. */
    phone_number_p = &person_p->phones.items_p[1];
    WARN_ON(strcmp(phone_number_p->number_p, "+46999999999") != 0);
    WARN_ON(phone_number_p->type != address_book_person_work_e);

    return (0);
}

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

    char buffer[100];
    int pid, res;

    // Receive msg
    nlh = (struct nlmsghdr *)skb->data;
    pid = nlh->nlmsg_pid;	/*pid of sending process */
    pr_info("received from PID %d:\n" "\"%s\"\n", pid, (char *)NLMSG_DATA(nlh));

    // execute
    encode_decode();

    sprintf(buffer, "End");
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
