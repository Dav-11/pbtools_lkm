#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/udp.h>
#include <linux/pkt_cls.h>
#include <bpf/bpf_endian.h>
#include <bpf/bpf_helpers.h>
#include <stdint.h>

#define ETH_P_IP    0x0800
#define PROTO_ICMP  1
#define PROTO_TCP   6
#define PROTO_UDP   17
#define PORT        60001

SEC("xdp")
int xdp_udp_header_extract(struct xdp_md *ctx) {

    void *data_end = (void *) (long) ctx->data_end;
    void *data = (void *) (long) ctx->data;

    struct ethhdr *eth = data;
    struct iphdr *ip;
    struct udphdr *udp;

    /*
     * Check if the address of the start of data (data)
     * + the size of the eth header is >
     * than the address of the end of the data (data_end)
     * => if the data is big enough to contain the eth header
     */
    if (data + sizeof(struct ethhdr) > data_end) {
        return XDP_DROP;
    }

    /*
     * Check if packet is IP
     * htons() -> host bytes to network bytes (Host TO Network Short)
     */
    if (eth->h_proto != bpf_ntohs(ETH_P_IP)) {
        return XDP_PASS;
    }

    // sets iphdr start after ethhdr end
    ip = (struct iphdr *)(data + sizeof(struct ethhdr));

    // get only packet sent to localhost
    if(ip->daddr != bpf_htonl(INADDR_LOOPBACK)) {
        return XDP_PASS;
    }

    /*
     * Check if the packet is big enough to contain iphdr
     */
    if (data + sizeof(struct ethhdr) + sizeof(struct iphdr) > data_end) {
        return XDP_PASS;
    }

    // Check if packet is UDP
    if (ip->protocol != IPPROTO_UDP) {
        return XDP_PASS;
    }

    /*
     * Check if the packet is big enough to contain udphdr
     */
    if (data + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) > data_end) {
        return XDP_PASS;
    }

    udp = (struct udphdr *)(data + sizeof(struct ethhdr) + sizeof(struct iphdr));

    // get only packet with my dest port
    if (udp->dest != bpf_htons(PORT)) {
        return XDP_PASS;
    }

    // get payload bytes = data_end - last address of last header (??)
    int payload_bytes = ((long) udp + (long) sizeof(struct udphdr)) - (long) data_end;

    uint8_t payload[payload_bytes];

    const char fmt_str[] = "received data: %s\n";

    // print string
    bpf_trace_printk(fmt_str, sizeof(fmt_str), (char *) payload);

}

char _license[] SEC("license") = "GPL";