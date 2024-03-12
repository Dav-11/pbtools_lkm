#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

MODULE_AUTHOR("Davide Collovigh");
MODULE_DESCRIPTION("netfilter_example_lkm: example kfunc declaration");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

static struct nf_hook_ops nfho;         //struct holding set of hook function options

//function to be called by hook
unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{

    struct iphdr *iph;

    if (!skb) return NF_ACCEPT;

    iph = ip_hdr(skb);

    // consider only tcp (grpc is tcp)
    if (iph->protocol == IPPROTO_TCP) {

		return NF_DROP;
	}

    return NF_ACCEPT;
}

static int __init pbtools_lkm_init(void)
{
    pr_info("Loaded module\n");

    nfho.hook       = (nf_hookfn*)hook_func;    /* hook function */
    nfho.hooknum    = NF_IP_LOCAL_IN;           /* packets to this machine */
    nfho.pf         = PF_INET;                  /* IPv4 */
    nfho.priority   = NF_IP_PRI_LAST;           /* min hook priority */
    
    nf_register_net_hook(&nfho);

    pr_info("Registered nethook function\n");

    return 0;
}

static void __exit pbtools_lkm_exit(void)
{
    pr_info("Removed module\n");
}

module_init(pbtools_lkm_init);
module_exit(pbtools_lkm_exit);