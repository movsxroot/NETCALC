# netcalc

A small C program that computes network information from an IP address and CIDR.

## Features

- Computes the subnet mask
- Computes the network address
- Counts the number of usable hosts
- Determines whether the IP is public or private

## Build

```bash
gcc main.c -o netcalc
```

## Usage

```bash
./netcalc <IP>/<CIDR>
```

### Example

```bash
./netcalc 192.168.5.189/24
```

Output:

```
IP              : 192.168.5.189/24
Decimal Mask    : 255.255.255.0
Type            : PRIVATE IP
Network Bits    : 24
Host Bits       : 8
Network Address : 192.168.5.0/24
Usable Hosts    : 254
```

