// SPDX-License-Identifier: MIT

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

{{if .ProtoContainsFloat }}
#include <asm/uaccess.h>
#include <asm/fpu/api.h>
{{ end }}

#include "{{ .PathToGenerated }}"

MODULE_AUTHOR("Your Name Here");
MODULE_DESCRIPTION("{{ .ModuleName }}: some description");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define MYPORT          60001
#define MAX_PAYLOAD_LEN 1024

//struct holding set of hook function options
static struct nf_hook_ops nfho;

typedef struct {
    uint32_t size;
    unsigned char encoded[MAX_PAYLOAD_LEN];
} message;


unsigned int process_message(message *data)
{

    /*
     * TODO: Place your code here
     */

    return NF_ACCEPT;
}

unsigned int handle_tcp_payload(struct sk_buff *skb)
{
    struct iphdr *ip;
    struct tcphdr *tcp;

    unsigned char *payload;
    char str[MAX_PAYLOAD_LEN];
    memset(str, 0, MAX_PAYLOAD_LEN);

    ip = ip_hdr(skb); // Update IP header pointer
    tcp = tcp_hdr(skb); // Update TCP header pointer

    uint32_t payload_len    = ntohs(ip->tot_len) - (ip->ihl * 4) - (tcp->doff * 4);

    pr_info("Payload length: %d\n", payload_len);

    if (payload_len > 0) {

        payload = (unsigned char *)(tcp) + (tcp->doff * 4);
        payload_len = min(payload_len, MAX_PAYLOAD_LEN); // Limit payload length to avoid excessive printing

        // create message struct
        message data;
        memset(&data, 0, sizeof(data));

        // copy in data.encoded memory from payload excluding first 4 bytes (len)
        memcpy(data.encoded, payload + 4, payload_len - 4);

        // get size (first 4 bytes)
        data.size = (uint32_t) ntohl(*((uint32_t *) payload));
        pr_info("Found size: %u\n", data.size);

        return process_message(&data);
    }

    return NF_ACCEPT;
}

unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *ip;
    struct tcphdr *tcp;

    int non_linear = 0;
    struct sk_buff *skb_linear;

    if (!skb) return NF_ACCEPT;

    ip = ip_hdr(skb);

    // consider only tcp
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

                //pr_info("is_nonlinear: %d", non_linear);

                skb_linear = skb_copy(skb, GFP_ATOMIC); // Make a copy to preserve the original skb
                if (!skb_linear) {
                    printk(KERN_ERR "Failed to allocate linearized skb\n");
                    return NF_DROP; // Drop packet if copy fails
                }

                // linearize
                int ret = skb_linearize(skb_linear);
                if (ret != 0) {

                    printk(KERN_ERR "Failed to linearize skb\n");
                    kfree_skb(skb_linear); // Free the linearized skb
                    return NF_DROP; // Drop packet if linearization fails
                }

                // Process the linearized skb
                return handle_tcp_payload(skb_linear);

            } else {

                return handle_tcp_payload(skb);
            }
        }
	}

    return NF_ACCEPT;
}

static int __init {{ .ModuleName }}_init(void)
{
    pr_info("Loaded module\n");

    nfho.hook       = (nf_hookfn*)hook_func;    /* hook function */
    nfho.hooknum    = NF_INET_LOCAL_IN;         /* packets to this machine */
    nfho.pf         = PF_INET;                  /* IPv4 */
    nfho.priority   = NF_IP_PRI_FIRST;          /* min hook priority */

    nf_register_net_hook(&init_net, &nfho);
    pr_info("Registered nethook function\n");

    return 0;
}

static void __exit {{ .ModuleName }}_exit(void)
{
    nf_unregister_net_hook(&init_net, &nfho);
    pr_info("Removed module\n");
}

module_init({{ .ModuleName }}_init);
module_exit({{ .ModuleName }}_exit);
