# address_book (UDP socket)

This example receives a udp message from userspace and decodes it and prints some part of it.

## Instructions
1. Compile module
```bash
make -C module
```
2. Install module
```bash
make -C module install
```
3. Load module
```bash
sudo insmod module/address_book.ko &
```
> IDK why but using make target for this command breaks often
4. Compile and run userspace app
```bash
make -C user_space
```
5. Unload the module
```bash
make -C module unload
```
5. You should see logs using dmesg
```bash
root@ubuntu:~# dmesg
[  326.742819] address_book:pbtools_lkm_init(): Loaded module
[  332.166743] address_book:pbtools_lkm_init(): Received data, decoding...
[  332.166749] address_book:decode(): people.length: 1
[  332.166749] address_book:decode(): person_p->id: 56
[  332.166750] address_book:decode(): phone_number_p->number_p: +46701232345
[  356.315550] address_book:pbtools_lkm_exit(): removed
```

