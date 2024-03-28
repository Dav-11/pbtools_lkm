# Source Library
The project is based (and is a fork of) [eerimoq/pbtools][pbtools] and shares its limitations:

- Options, services (gRPC) and reserved fields are ignored.
- Public imports are not implemented.

## Changes from source

### Float
Since floating points types are not available on every kernel they have been replaced with:

| type   | substitution |
| ------ | ------------ |
| float  | uint32_t     |
| double | uint64_t     |