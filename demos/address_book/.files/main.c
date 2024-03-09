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

#define MY_TCP_PORT         4445
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
static struct socket *new_sock;	/* communication socket */


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

// static void process_received_data(struct msghdr *msg) {
//     struct kvec *iov = msg.msg_iov;
//     int iov_len = msg.msg_iovlen;

//     // Iterate through the iovec array
//     for (int i = 0; i < iov_len; i++) {
        
//         // Check if the current buffer contains at least the size of message struct
//         if (iov[i].iov_len >= sizeof(message)) {
            
//             // Extract the message struct from the buffer
//             message *msg_ptr = (message *)iov[i].iov_base;

//             // Access fields of the message struct
//             int size = msg_ptr->size;
//             uint8_t *encoded_data = msg_ptr->encoded;

//             // Process the received message
//             // Example: Print the size and contents of the encoded data
//             printk(KERN_INFO "Received message size: %d\n", size);
//             printk(KERN_INFO "Encoded data: ");
//             for (int j = 0; j < size; j++) {
//                 printk(KERN_CONT "%02x ", encoded_data[j]);
//             }
//             printk(KERN_CONT "\n");

//             // You can break here if you only want to process the first message found
//             break;
//         }
//     }
// }

static int __init pbtools_lkm_init(void)
{

    int err;

	/* address to bind on */
	struct sockaddr_in addr = {
		.sin_family	= AF_INET,
		.sin_port	= htons(MY_TCP_PORT),           // translate into network bits representation (short)
		.sin_addr	= { htonl(INADDR_LOOPBACK) }    // translate into network bits representation (long) 127.0.0.1
	};

    /* address of peer */
	struct sockaddr_in raddr;

    // init vars
    sock = NULL;
	new_sock = NULL;
	err = 0;

    pr_info("Loaded module");

    // create listening socket
	err = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
	if (err < 0) {
		
        pr_err("Could not create socket");
		goto out;
	}

    // reset err
    err = 0;

    // bind socket to loopback on port MY_TCP_PORT
	err = sock->ops->bind (sock, (struct sockaddr *) &addr, sizeof(addr));
	if (err < 0) {
	
		/* handle error */
		pr_err("Could not bind socket");
		goto out;
	}

    // reset err
    err = 0;

    // start listening
    // backlog is the size of the queue for the socket, when full system rejects pkg
	// https://tangentsoft.com/wskfaq/advanced.html#backlog
	err = sock->ops->listen (sock, LISTEN_BACKLOG);
	if (err < 0) {
	
		/* handle error */
		pr_err("Could not listen from socket");
		goto out;
	}

    // reset err
    err = 0;

    // create new socket for the accepted connection
    err = sock_create_lite(PF_PACKET, sock->type, IPPROTO_TCP, &new_sock);
	if (err) {

		pr_err("Can't allocate socket x accept\n");
		goto out_release;
	}

    // copy sock_ops struct to new socket
	new_sock->ops = sock->ops;

    err = 0;

    // accept a connection
    err = sock->ops->accept(sock, new_sock, 0, true);
	if (err) {

		pr_err("Could not accept connection from socket");
		goto out_release;
	}


    //
    // Now new_sock is the accepted connection
    //

    // Read data from the socket
    char buf[1024];

    struct kvec iov;
    struct msghdr msg;
    int flags = MSG_WAITALL; // Or other flags as needed

    memset(&iov, 0, sizeof(iov));
    memset(&msg, 0, sizeof(msg));

    // init msghdr
    msg.msg_name = (struct sockaddr *) &raddr;
	msg.msg_namelen = sizeof(raddr);

    // init kvec
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);

    // Call kernel_recvmsg() to receive data
    err = kernel_recvmsg(sock, &msg, &iov, 1, iov.iov_len, flags);

    if (err > 0) {

        // Process received data
        // Do something with buffer
        pr_info("Received: %s", buf);

    } else if (err == 0) {

        // Connection closed
        pr_err("Connection Closed! err: %d", err);
        goto out_release_new_sock;

    } else {

        // Error handling
        pr_err("Error in rcvmsg! err: %d", err);
        goto out_release_new_sock;
    }

    pr_info("done");

    return 0;

out_release_new_sock:

	/* cleanup socket for accepted connection */
	sock_release(new_sock);

out_release:

	/* cleanup listening socket */
	sock_release(sock);
out:
	return err;
}

static void __exit pbtools_lkm_exit(void)
{
    pr_info("removed\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);
