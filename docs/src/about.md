# About
[protocol-buffers]: https://developers.google.com/protocol-buffers
[proto3]: https://developers.google.com/protocol-buffers/docs/proto3
[pbtools]: https://github.com/eerimoq/pbtools

[Google Protocol Buffers][protocol-buffers] implementation that runs as a linux kernel module.

The project is based (and is a fork of) [eerimoq/pbtools][pbtools] and shares its limitations:

- Options, services (gRPC) and reserved fields are ignored.
- Public imports are not implemented.

