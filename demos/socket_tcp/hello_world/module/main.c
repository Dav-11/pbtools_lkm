// SPDX-License-Identifier: MIT

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/fs.h>
#include <net/sock.h>

#include "generated/hello_world.h"

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("pbtools_lkm_main: protobuf LKM");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define MY_UDP_PORT         60001
#define LISTEN_BACKLOG		5       // queue length for port


static struct socket *sock;	/* listening (server) socket */
static struct socket *new_sock;	/* communication socket */


// message struct to be sent
typedef struct {
    int size;
    uint8_t encoded[128];
} message;

/**
 * This function decode the protobuf message and extracts bar
*/
static int get_bar(message *data)
{
    uint8_t workspace[1024];

    struct hello_world_foo_t *hello_world_str;

    /* Decode the message. */
    hello_world_str = hello_world_foo_new(&workspace[0], sizeof(workspace));
    WARN_ON(hello_world_str == NULL);

    hello_world_foo_decode(hello_world_str, &data->encoded[0], data->size);

    return (int) hello_world_str->bar;
}

static int __init pbtools_lkm_init(void)
{

    int err;
	err = 0;


    message data;
    memset(&data, 0, sizeof(data));

	/* address to bind on */
	struct sockaddr_in addr = {
		.sin_family	= AF_INET,
		.sin_port	= htons(MY_UDP_PORT),
		.sin_addr	= { htonl(INADDR_LOOPBACK) }
	};

    sock = NULL;

    pr_info("Loaded module");

    // create listening socket
	err = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
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

    // start listening
    // backlog is the size of the queue for the socket, when full system rejects pkg
	// https://tangentsoft.com/wskfaq/advanced.html#backlog
	err = sock->ops->listen (sock, LISTEN_BACKLOG);
	if (err < 0) {
	
		/* handle error */
		pr_err("Could not listen from socket");
		goto out;
	}

    // create socket x accept connection
    int rc = sock_create_lite(PF_PACKET, sock->type, IPPROTO_TCP, &new_sock);
	if (rc) {

		pr_err("Can't allocate socket x accept\n");
		goto out_release;
	}

    // copy struct 
	new_sock->ops = sock->ops;

	// accept a connection
	rc = sock->ops->accept(sock, new_sock, 0, true);
	if (rc) {

		pr_err("Could not accept connection from socket");
		goto out_release;
	}

    int bar;

    do {

        // receive protobuf
        struct msghdr msg;
        struct kvec iov;

        memset(&msg, 0, sizeof(struct msghdr));
        memset(&iov, 0, sizeof(struct kvec));

        iov.iov_base = data.encoded;
        iov.iov_len = sizeof(data.encoded);

        err = kernel_recvmsg(new_sock, &msg, &iov, 1, 1024, MSG_WAITALL);
        if (err < 0) {

            pr_err("Failed to receive TCP data (protobuf): %d\n", err);
            goto out_release_new_sock;
        }

        data.size = sizeof(data.encoded);

        pr_info("Received data, decoding... \n");

        // Process received data as needed
        bar = get_bar(&data);

        pr_info("received bar: %d", bar);

        if (bar <= 5) pr_info("bar: %d <= 5 so keep on looping", bar);

    } while (bar <= 5);

    pr_info("exit the loop");

out_release_new_sock:

	/* cleanup socket for accepted connection */
	sock_release(new_sock);

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
