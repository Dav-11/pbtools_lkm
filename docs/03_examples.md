# Examples

Inside the (Examples)[] folder there are some examples on how this project can be used. In particular there are:

## UDP Sockets
Here an UDP socket is opened inside the LKM to receive a protobuf message, that is then decoded and printed to dmesg.

## Netfilter
Here the module is used to filter tcp packets between two applications and apply some drop logics based on the protobuf content.