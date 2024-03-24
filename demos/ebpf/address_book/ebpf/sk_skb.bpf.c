#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <stddef.h>
#include <bpf/bpf_endian.h>
#include <bpf/bpf_helpers.h>
#include <linux/pkt_cls.h>

#define MYPORT 60001


SEC("sk_skb")
int bpf_program(struct __sk_buff *skb) {

    void *data_end = (void *) (long) skb->data_end;
    void *data = (void *) (long) skb->data;

    struct ethhdr *eth = data;
    struct iphdr *ip;

    /*
     * Check if the address of the start of data (data)
     * + the size of the eth header is >
     * than the address of the end of the data (data_end)
     * => if the data is big enough to contain the eth header
     */
    if (data + sizeof(struct ethhdr) > data_end) {
        return TC_ACT_OK;
    }

    // Check if the packet is IPv4
    if (skb->protocol == bpf_htons(ETH_P_IP)) {

        // sets iphdr start after ethhdr end
        ip = (struct iphdr *)(data + sizeof(struct ethhdr));

        // Check if the packet is TCP & dest address is localhost
        if (ip->protocol == IPPROTO_TCP && ip->daddr == bpf_htonl(INADDR_LOOPBACK)) {

            struct tcphdr *tcp = (struct tcphdr *)(ip + 1);

            // Access fields of TCP header
            uint16_t src_port = ntohs(tcp->source);
            uint16_t dest_port = ntohs(tcp->dest);

            // Logic based on TCP ports, you can implement your custom logic here
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
                bpf_printk("Received TCP packet. Source IP: %u.%u.%u.%u, Destination IP: %u.%u.%u.%u\n",NIPQUAD(ip->saddr), NIPQUAD(ip->daddr));
                bpf_printk("Payload length: %u\n", payload_len);

                // Do something, e.g., drop the packet
                return TC_ACT_SHOT;
            }
        }
    }
    // Allow the packet to continue its journey
    return TC_ACT_OK;
}

char _license[] SEC("license") = "GPL";