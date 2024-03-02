# Common errors

## Skipping BTF generation for <path>.ko due to unavailability of vmlinux
### Solution
```shell
apt install dwarves
cp /sys/kernel/btf/vmlinux /usr/lib/modules/`uname -r`/build/
```
> Of course, you first have to check that your vmlinux is indeed in `/sys/kernel/btf`

### Source
- https://askubuntu.com/questions/1348250/skipping-btf-generation-xxx-due-to-unavailability-of-vmlinux-on-ubuntu-21-04