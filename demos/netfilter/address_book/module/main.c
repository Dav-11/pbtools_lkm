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

#include "generated/address_book.h"

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("netfilter_example: address_book protobuf");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define MYPORT          60001
#define MAX_PAYLOAD_LEN 1024 // Maximum length of payload to print


static struct nf_hook_ops nfho;         //struct holding set of hook function options

typedef struct {
    uint32_t size;
    unsigned char encoded[MAX_PAYLOAD_LEN];
} message;

struct address_book_address_book_t *decode_address_book(message *data, uint8_t *workspace)
{
    struct address_book_address_book_t *address_book_p;
    struct address_book_person_t *person_p;
    struct address_book_person_phone_number_t *phone_number_p;

    /* Decode the message. */
    address_book_p = address_book_address_book_new(&workspace[0], sizeof(workspace));
    WARN_ON(address_book_p == NULL);

    int size = address_book_address_book_decode(address_book_p, &data->encoded[0], data->size);
    //WARN_ON(size < 0);
    WARN_ON(address_book_p->people.length != 1);

    pr_info("people.length: %d", address_book_p->people.length);

    /* Check the decoded person. */
    person_p = &address_book_p->people.items_p[0];
    WARN_ON(strcmp(person_p->name_p, "Kalle Kula") != 0);
    WARN_ON(person_p->id != 56);
    pr_info("person_p->id: %d", person_p->id);

    WARN_ON(strcmp(person_p->email_p, "kalle.kula@foobar.com") != 0);
    WARN_ON(person_p->phones.length != 2);

    /* Check home phone number. */
    phone_number_p = &person_p->phones.items_p[0];
    WARN_ON(strcmp(phone_number_p->number_p, "+46701232345") != 0);
    WARN_ON(phone_number_p->type != address_book_person_home_e);
    pr_info("phone_number_p->number_p: %s", phone_number_p->number_p);

    /* Check work phone number. */
    phone_number_p = &person_p->phones.items_p[1];
    WARN_ON(strcmp(phone_number_p->number_p, "+46999999999") != 0);
    WARN_ON(phone_number_p->type != address_book_person_work_e);
}

int handle_tcp_payload(struct sk_buff *skb)
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

        // print full payload in hex
        pr_info("Payload:\n");
        print_hex(payload, payload_len);

        // create message struct
        message data;
        memset(&data, 0, sizeof(data));

        // copy in data.encoded memory from payload excluding first 4 bytes (len)
        memcpy(data.encoded, payload + 4, payload_len - 4);

        // get size (first 4 bytes)
        data.size = (uint32_t) ntohl(*((uint32_t *) payload));

        pr_info("Found size: %u\n", data.size);

        pr_info("Data.encoded:\n");
        print_hex(data.encoded, strlen(data.encoded));

        int bar = get_bar(&data);
        pr_info("received bar: %d", bar);

        return bar;
    }

    return -1;
}

//function to be called by hook
unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{

    struct iphdr *ip;
    struct tcphdr *tcp;

    int non_linear = 0;
    struct sk_buff *skb_linear;

    int bar = -1;
    uint8_t workspace[1024];

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

                //pr_info("is_nonlinear: %d", non_linear);
                
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
                bar = handle_tcp_payload(skb_linear);

            } else {

                bar = handle_tcp_payload(skb);
            }

            /*
             * Handle protobuf
             * 
             */
            if (bar > 50) {

                pr_info("bar > 50: dropped pkg");
                return NF_DROP;
            }
        }
	}

    return NF_ACCEPT;
}

static int __init pbtools_lkm_init(void)
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

static void __exit pbtools_lkm_exit(void)
{
    nf_unregister_net_hook(&init_net, &nfho);
    pr_info("Removed module\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);
