// SPDX-License-Identifier: MIT

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/tcp.h>

#include "generated/address_book.h"

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("pbtools_lkm_main: protobuf LKM");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define PORT    4445

// Define your struct
typedef struct {
    int size;
    uint8_t encoded[128];
} encoded_data;

static void decode(encoded_data *data)
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

    struct socket *sock, *new_sock;
    struct sockaddr_in sin;
    int ret;

    // Create socket
    ret = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
    if (ret < 0) {
        printk(KERN_ERR "Failed to create socket\n");
        return ret;
    }

    printk(KERN_INFO "Created socket\n");

    // Set up the address structure
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Example: Bind to localhost
    sin.sin_port = htons(PORT); // Example port

    // Bind the socket
    ret = sock->ops->bind(sock, (struct sockaddr *)&sin, sizeof(sin));
    if (ret < 0) {
        printk(KERN_ERR "Failed to bind socket\n");
        sock_release(sock);
        return ret;
    }
    printk(KERN_INFO "Bound socket\n");

    // Listen on the socket
    ret = sock->ops->listen(sock, 10); // Example backlog of 10 connections
    if (ret < 0) {
        printk(KERN_ERR "Failed to listen on socket\n");
        sock_release(sock);
        return ret;
    }

    // Accept incoming connections
    ret = sock->ops->accept(sock, new_sock, 0, true);
    if (ret < 0) {
        printk(KERN_ERR "Failed to accept connection\n");
        sock_release(sock);
        return ret;
    }

    //
    // Now new_sock is the accepted connection
    //

    // Read data from the socket
    char buf[1024];

    struct kvec vec;
    struct msghdr msg;
    memset(&vec, 0, sizeof(vec));
    memset(&msg, 0, sizeof(msg));

    vec.iov_base = buf;
    vec.iov_len = sizeof(buf);
    
    ret = kernel_recvmsg(new_sock, &msg, &vec, 1, sizeof(buf), 0);
    if (ret < 0) {
        printk(KERN_ERR "Failed to receive data\n");
        sock_release(new_sock);
        sock_release(sock);
        return ret;
    }

    // Process received data
    printk(KERN_INFO "Received data: %s\n", buf);

    // Cleanup
    sock_release(new_sock);
    sock_release(sock);

    pr_info("inserted\n");

    return 0;		/* success */
}

static void __exit pbtools_lkm_exit(void)
{
    pr_info("removed\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);
