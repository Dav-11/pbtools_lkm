// SPDX-License-Identifier: MIT

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/fs.h>
#include <net/sock.h>

#include "generated/address_book.h"

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("pbtools_lkm_main: protobuf LKM");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define MY_UDP_PORT         60001
#define LISTEN_BACKLOG		5       // queue length for port


// MACRO to print peer address in connection
#define print_sock_address(addr)						\
	do {												\
		printk(LOG_LEVEL "connection established to "	\
			"%pI4:%d\n",	 							\
			&addr.sin_addr.s_addr,						\
			ntohs(addr.sin_port));						\
	} while (0)


static struct socket *sock;	/* listening (server) socket */
//static struct socket *new_sock;	/* communication socket */


// message struct to be sent
typedef struct {
    int size;
    uint8_t encoded[128];
} message;

static void decode(message *data)
{
    uint8_t workspace[1024];

    struct address_book_address_book_t *address_book_p;
    struct address_book_person_t *person_p;
    struct address_book_person_phone_number_t *phone_number_p;

    /* Decode the message. */
    address_book_p = address_book_address_book_new(&workspace[0], sizeof(workspace));
    WARN_ON(address_book_p == NULL);

    int size = address_book_address_book_decode(address_book_p, &data->encoded[0], data->size);
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
}

static int __init pbtools_lkm_init(void)
{

    int err;
    message message;

	/* address to bind on */
	struct sockaddr_in addr = {
		.sin_family	= AF_INET,
		.sin_port	= htons(MY_UDP_PORT),
		.sin_addr	= { htonl(INADDR_LOOPBACK) }
	};

    // init vars
    sock = NULL;
	err = 0;
    memset(&message, 0, sizeof(message));


    pr_info("Loaded module");

    // create listening socket
	err = sock_create_kern(&init_net, PF_INET, SOCK_DGRAM, IPPROTO_UDP, &sock);
	if (err < 0) {
		
        pr_err("Could not create socket: %d", err);
		goto out;
	}

    // reset err
    err = 0;

    // bind socket to loopback on port
    err = sock->ops->bind (sock, (struct sockaddr *) &addr, sizeof(addr));
	if (err < 0) {
	
		/* handle error */
		pr_err("Could not bind socket: %d", err);
		goto out;
	}

    // reset err
    err = 0;

    // data struct
    struct msghdr msg;
	struct kvec iov;

    memset(&msg, 0, sizeof(struct msghdr));
    memset(&iov, 0, sizeof(struct kvec));


    iov.iov_base = &message; // Allocate struct for receiving data
    iov.iov_len = sizeof(message);

    err = kernel_recvmsg(sock, &msg, &iov, 1, 1024, MSG_WAITALL);
    if (err < 0) {
        pr_err("Failed to receive UDP data: %d\n", err);
        goto out_release;
    }

    pr_info("Received data, decoding... \n");

    // Process received data as needed
    decode(&message);

out_release:

	/* cleanup listening socket */
	sock_release(sock);
out:
	return 0;
}

static void __exit pbtools_lkm_exit(void)
{
    pr_info("removed\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);
