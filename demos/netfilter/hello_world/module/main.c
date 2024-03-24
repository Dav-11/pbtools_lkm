#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <linux/skbuff.h>

//#include "generated/hello_world.h"

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("netfilter_example: example tcp netfilter");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define MYPORT 60001

static struct nf_hook_ops nfho;         //struct holding set of hook function options

typedef struct {
    int size;
    char encoded[128];
} message;

/**
 * This function decode the struct message and extracts bar
 */
// static int get_bar(message *data)
// {
//     uint8_t workspace[1024];

//     struct hello_world_foo_t *hello_world_str;

//     /* Decode the message. */
//     hello_world_str = hello_world_foo_new(&workspace[0], sizeof(workspace));
//     WARN_ON(hello_world_str == NULL);

//     hello_world_foo_decode(hello_world_str, &data->encoded[0], data->size);

//     return (int) hello_world_str->bar;
// }

//function to be called by hook
unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{

    struct iphdr *ip;
    struct tcphdr *tcp;
    unsigned char *payload;
    char str[1024];
    memset(str, 0, 1024);

    if (!skb) return NF_ACCEPT;

    ip = ip_hdr(skb);

    // consider only tcp (grpc is tcp)
    if (ip->protocol == IPPROTO_TCP && ip->daddr == htonl(INADDR_LOOPBACK)) {

        // get header tcp
        tcp = tcp_hdr(skb);

        // Access fields of TCP header
        uint16_t src_port = ntohs(tcp->source);
        uint16_t dest_port = ntohs(tcp->dest);

        if (dest_port == MYPORT && tcp->psh) {

            pr_info("Received TCP packet. Source Port:%d, Destination Port:%d PUSH: %d\n", src_port, dest_port, tcp->psh);

            /*
             * Calculate the offset to the beginning of the TCP payload
             * ip->ihl : "Internet Header Length" length of the header in 32-bit (4 bytes) words
             * tcp->doff : "Data Offset" length of the TCP header in 32-bit (4 bytes) words
             */
            // uint32_t payload_offset = sizeof(struct ethhdr) + ip->ihl * 4 + tcp->doff * 4;
            uint32_t payload_offset = ip->ihl * 4 + tcp->doff * 4;

            // Accessing the payload
            //payload = (unsigned char *)ip + payload_offset;
            uint32_t payload_len = skb->len - payload_offset;


            // Debug
            pr_info("Payload length: %d\n", payload_len);

            if (skb_is_nonlinear(skb)) {

                pr_info("is_nonlinear: %d", 1);
                uint32_t non_paged_data = skb->len - skb->data_len;
                pr_info("non_paged_data: %u", non_paged_data);
                pr_info("payload_offset: %u", payload_offset);

                void *temp_buffer = kmalloc(payload_offset, GFP_ATOMIC);
                if (!temp_buffer) {
                    return NF_ACCEPT; // Allocation failed, drop the packet
                }

                // Copy data using skb_copy_bits
                if (skb_copy_bits(skb, payload_offset, temp_buffer, payload_len) < 0) {
                
                    return NF_ACCEPT; // Copy failed, drop the packet
                }

                memcpy(str, temp_buffer + tcph->doff * 4, sizeof(payload));

            } else {

                payload = (unsigned char *)(skb->data + payload_offset);
                memcpy(str, payload, payload_len);
            }

            for(int i = 0; i < strlen(str); i++)
            {
                pr_info("payload[%d]: %X\n", i, (char) payload[i]);
            }
            pr_info("TCP payload: %s\n", str);
            
            
            //pr_info("strlen: %d", strlen(payload));

            

            //message data;
            //memset(&data, 0, sizeof(data));
            //memcpy(data.encoded, payload, payload_len);

            //pr_info("Intercepted message: %s \n", data.encoded);

            // Process received data as needed
            //int bar = get_bar(&data);

            //pr_info("received bar: %d", bar);
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
    nfho.priority   = NF_IP_PRI_FIRST;           /* min hook priority */
    
    nf_register_net_hook(&init_net, &nfho);

    pr_info("Registered nethook function\n");

    return 0;
}

static void __exit pbtools_lkm_exit(void)
{
    nf_unregister_net_hook(&init_net, &nfho);
    pr_info("Removed module\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);