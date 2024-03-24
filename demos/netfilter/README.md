# Netfilter

Still WIP

```shell
tcpdump -X -i lo -s 0 'dst port 60001'
```

```
19:05:39.328301 IP localhost.48230 > localhost.60001: Flags [P.], seq 0:18, ack 1, win 512, options [nop,nop,TS val 1802058821 ecr 1802058821], length 18
	0x0000:  4500 0046 d325 4000 4006 698a 7f00 0001  E..F.%@.@.i.....
	0x0010:  7f00 0001 bc66 ea61 c903 5307 49e7 afbc  .....f.a..S.I...
	0x0020:  8018 0200 fe3a 0000 0101 080a 6b69 3c45  .....:......ki<E
	0x0030:  6b69 3c45 4865 6c6c 6f20 6672 6f6d 2063  ki<EHello.from.c
	0x0040:  6c69 656e 7421                           lient!
```