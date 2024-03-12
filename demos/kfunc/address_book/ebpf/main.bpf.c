#include <stddef.h>
#include <linux/bpf.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>

#define ETH_P_IP    0x0800

#define PROTO_ICMP  1
#define PROTO_TCP   6
#define PROTO_UDP   17

/* Header cursor to keep track of current parsing position */
struct hdr_cursor {
    void *pos;
};

SEC("xdp")
int  xdp_parser_func(struct xdp_md *ctx)
{

    void *data = (void *)(long)ctx->data;
    void *data_end = (void *)(long)ctx->data_end;

    struct ethhdr *eth = data;

    if (data + sizeof(struct ethhdr) > data_end) return 0;

    // Check that it's an IP packet
    if (bpf_ntohs(eth->h_proto) == ETH_P_IP)
    {
        // Return the protocol of this packet
        // 1 = ICMP
        // 6 = TCP
        // 17 = UDP
        struct iphdr *iph = data + sizeof(struct ethhdr);

        if ((data + sizeof(struct ethhdr) + sizeof(struct iphdr) <= data_end) && (iph->protocol == PROTO_TCP)) {



        }
    }

    return XDP_PASS;

}

char _license[] SEC("license") = "GPL";