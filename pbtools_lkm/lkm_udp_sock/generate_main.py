import os

MAIN_NF_FMT = '''
// SPDX-License-Identifier: MIT

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/fs.h>
#include <net/sock.h>

#include "{import_path}"

MODULE_AUTHOR("Your Name Here");
MODULE_DESCRIPTION("{module_name}: some description");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define MY_UDP_PORT         60001
#define LISTEN_BACKLOG      5       // queue length for port
#define BUFFER_SIZE         128

static struct socket *sock;    /* listening (server) socket */

// message struct to be sent
typedef struct {{
    int size;
    uint8_t encoded[BUFFER_SIZE];
}} message;

// Function to print payload
void print_hex(const unsigned char *payload, unsigned int payload_len)
{{
    unsigned int i;
    for (i = 0; i < payload_len; ++i) {{
        pr_info("%02x -> %c\\n", payload[i], payload[i]);
    }}
    pr_info("\\n");
}}

static void decode(message *data)
{{
    /*
     * TODO: Place your code here
     */
     
    pr_info("Data.encoded:\n");
    print_hex(data.encoded, strlen(data.encoded));

    pr_info("To implement");
}}

static int __init {module_name}_init(void)
{{
    int err;
    err = 0;

    message data;
    memset(&data, 0, sizeof(data));

    /* address to bind on */
    struct sockaddr_in addr = {{
        .sin_family    = AF_INET,
        .sin_port    = htons(MY_UDP_PORT),
        .sin_addr    = {{ htonl(INADDR_LOOPBACK) }}
    }};

    sock = NULL;

    pr_info("Loaded module");

    // create listening socket
    err = sock_create_kern(&init_net, PF_INET, SOCK_DGRAM, IPPROTO_UDP, &sock);
    if (err < 0) {{

        pr_err("Could not create socket: %d", err);
        goto out;
    }}

    // reset err
    err = 0;

    // bind socket to loopback on port
    err = sock->ops->bind (sock, (struct sockaddr *) &addr, sizeof(addr));
    if (err < 0) {{

        /* handle error */
        pr_err("Could not bind socket: %d", err);
        goto out;
    }}

    // reset err
    err = 0;

    // receive protobuf

    struct msghdr msg;
    struct kvec iov;

    memset(&msg, 0, sizeof(struct msghdr));
    memset(&iov, 0, sizeof(struct kvec));

    iov.iov_base = data.encoded;
    iov.iov_len = sizeof(data.encoded);

    err = kernel_recvmsg(sock, &msg, &iov, 1, 1024, MSG_WAITALL);
    if (err < 0) {{
        pr_err("Failed to receive UDP data (protobuf): %d\\n", err);
        goto out_release;
    }}

    data.size = sizeof(data.encoded);

    // Process received data as needed
    decode(&data);

out_release:

    /* cleanup listening socket */
    sock_release(sock);
out:
    return 0;
}}

static void __exit {module_name}_exit(void)
{{
    
    /* cleanup listening socket */
    sock_release(sock);
    
    pr_info("removed\\n");
}}

module_init({module_name}_init);
module_exit({module_name}_exit);
'''


def generate_main_udp(module_name, import_path, output_directory):

    filename = os.path.join(output_directory, 'main.c')

    with open(filename, 'w') as f:
        f.write(MAIN_NF_FMT.format(module_name=module_name, import_path=import_path))

