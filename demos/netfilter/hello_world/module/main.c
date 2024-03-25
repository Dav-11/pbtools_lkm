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
#define MAX_PAYLOAD_LEN 1024 // Maximum length of payload to print

static struct nf_hook_ops nfho;         //struct holding set of hook function options

typedef struct {
    int size;
    char encoded[MAX_PAYLOAD_LEN];
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

// Function to print TCP payload
void print_tcp_payload(const unsigned char *payload, unsigned int payload_len) {
    unsigned int i;
    pr_info("TCP Payload (first %u bytes):\n", payload_len);
    for (i = 0; i < payload_len; ++i) {
        pr_info("%02x -> %c\n", payload[i], payload[i]);
    }
    pr_info("\n");
}

void handle_tcp_payload(struct sk_buff *skb)
{
    struct iphdr *ip;
    struct tcphdr *tcp;

    unsigned char *payload;
    // char str[MAX_PAYLOAD_LEN];
    // memset(str, 0, MAX_PAYLOAD_LEN);

    ip = ip_hdr(skb); // Update IP header pointer
    tcp = tcp_hdr(skb); // Update TCP header pointer

    uint32_t payload_len    = ntohs(ip->tot_len) - (ip->ihl * 4) - (tcp->doff * 4);

    pr_info("Payload length: %d\n", payload_len);

    if (payload_len > 0) {

        payload = (unsigned char *)(tcp) + (tcp->doff * 4);
        payload_len = min(payload_len, MAX_PAYLOAD_LEN); // Limit payload length to avoid excessive printing
        
        print_tcp_payload(payload, payload_len);
    }
}

//function to be called by hook
unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{

    struct iphdr *ip;
    struct tcphdr *tcp;

    int non_linear = 0;
    struct sk_buff *skb_linear;

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
            
            // check if linear
            non_linear = skb_is_nonlinear(skb);
            if (non_linear) {

                pr_info("is_nonlinear: %d", non_linear);
                
                skb_linear = skb_copy(skb, GFP_ATOMIC); // Make a copy to preserve the original skb
                if (!skb_linear) {
                    printk(KERN_ERR "Failed to allocate linearized skb\n");
                    return NF_DROP; // Drop packet if linearization fails
                }

                // linearize
                int ret = skb_linearize(skb_linear);
                if (ret != 0) {
                    
                    printk(KERN_ERR "Failed to linearize skb\n");
                    kfree_skb(skb_linear); // Free the linearized skb
                    return NF_DROP; // Drop packet if linearization fails
                }

                // Process the linearized skb
                handle_tcp_payload(skb_linear);

            } else {

                handle_tcp_payload(skb);
            }
            

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