#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/ip.h>

#include "generated/hello_world.h"

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("netfilter_example: example tcp netfilter");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define MYPORT 60001

static struct nf_hook_ops nfho;         //struct holding set of hook function options

typedef struct {
    int size;
    uint8_t encoded[128];
} message;

/**
 * This function decode the struct message and extracts bar
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

//function to be called by hook
unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{

    struct iphdr *ip;
    struct tcphdr *tcp;

    if (!skb) return NF_ACCEPT;

    ip = ip_hdr(skb);

    // consider only tcp (grpc is tcp)
    if (ip->protocol == IPPROTO_TCP && ip->daddr == htonl(INADDR_LOOPBACK)) {

        // get header tcp
        tcp = tcp_hdr(skb);

        // Access fields of TCP header
        uint16_t src_port = ntohs(tcp->source);
        uint16_t dest_port = ntohs(tcp->dest);

        pr_info("Received TCP packet. Source Port:%d, Destination Port:%d\n", src_port, dest_port);

        if (dest_port == MYPORT) {

            /*
             * Calculate the offset to the beginning of the TCP payload
             * ip->ihl : "Internet Header Length" length of the header in 32-bit (4 bytes) words
             * tcp->doff : "Data Offset" length of the TCP header in 32-bit (4 bytes) words
             */
            uint32_t tcp_payload_offset = sizeof(struct ethhdr) + ip->ihl * 4 + tcp->doff * 4;

            // Accessing the payload
            void *payload = (void *)(skb->data + tcp_payload_offset);
            uint32_t payload_len = skb->len - tcp_payload_offset;

            // Debug
            pr_info("Payload length: %u\n", payload_len);

            message data;
            memset(&data, 0, sizeof(data));
            memcpy(data.encoded, payload, payload_len);

            pr_info("Received data, decoding... \n");

            // Process received data as needed
            int bar = get_bar(&data);

            pr_info("received bar: %d", bar);
        }

		//return NF_DROP;
	}

    return NF_ACCEPT;
}

static int __init pbtools_lkm_init(void)
{
    pr_info("Loaded module\n");

    nfho.hook       = (nf_hookfn*)hook_func;    /* hook function */
    nfho.hooknum    = NF_INET_LOCAL_IN;           /* packets to this machine */
    nfho.pf         = PF_INET;                  /* IPv4 */
    nfho.priority   = NF_IP_PRI_LAST;           /* min hook priority */
    
    nf_register_net_hook(&init_net, &nfho);

    pr_info("Registered nethook function\n");

    return 0;
}

static void __exit pbtools_lkm_exit(void)
{
    pr_info("Removed module\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);