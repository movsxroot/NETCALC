#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "ip.h"

int main(int argc, char *argv[]) {
    InfoIP ip;
    IPStatus status;

    if (argc != 2) {
        printUsage(argv[0]);
        return 1;
    }

    status = computeIP(&ip, argv[1]);

    switch (status) {
        case IP_OK:
            printIP(&ip);
            return 0;
        case IP_ERR_FORMAT:
            printf("Error: invalid format. Expected IP/CIDR, e.g. 192.168.5.189/24\n");
            return 1;
        case IP_ERR_CIDR:
            printf("Error: CIDR must be between %d and %d.\n", CIDR_MIN, CIDR_MAX);
            return 1;
    }
    return 1;
}

IPStatus parseStringIP(InfoIP *ip, const char *string) {
    unsigned int octet0, octet1, octet2, octet3;
    int cidr;
    int consumed = 0;
    int n = sscanf(string, "%u.%u.%u.%u/%d%n", &octet0, &octet1, &octet2, &octet3, &cidr, &consumed);
    if (n != 5) {
        return IP_ERR_FORMAT;
    }
    if (string[consumed] != '\0') {
        return IP_ERR_FORMAT;
    }
    if (octet0 > 255 || octet1 > 255 || octet2 > 255 || octet3 > 255) {
        return IP_ERR_FORMAT;
    }
    if (cidr < CIDR_MIN || cidr > CIDR_MAX) {
        return IP_ERR_CIDR;
    }
    ip->octetsIP[0] = (uint8_t)octet0;
    ip->octetsIP[1] = (uint8_t)octet1;
    ip->octetsIP[2] = (uint8_t)octet2;
    ip->octetsIP[3] = (uint8_t)octet3;
    ip->cidr = cidr;
    ip->networkBits = cidr;
    ip->hostBits = 32 - cidr;
    return IP_OK;
}

IPStatus computeIP(InfoIP *ip, const char *string) {
    IPStatus status = parseStringIP(ip, string);
    if (status != IP_OK) {
        return status;
    }
    computeMask(ip);
    computeNetworkAddress(ip);
    computeHostCount(ip);
    defineType(ip);
    computeBroadcast(ip);
    return IP_OK;
}

void defineType(InfoIP *ip) {
    if (ip->octetsIP[0] == 10 ||
        (ip->octetsIP[0] == 172 && ip->octetsIP[1] >= 16 && ip->octetsIP[1] <= 31) ||
        (ip->octetsIP[0] == 192 && ip->octetsIP[1] == 168))
        ip->type = PRIVATE;
    else
        ip->type = PUBLIC;
}

uint32_t maskFromCidr(int cidr) {
    if (cidr == 0) {
        return 0;
    }
    return 0xFFFFFFFFu << (32 - cidr);
}

void computeMask(InfoIP *ip) {
    uint32_t mask = maskFromCidr(ip->cidr);
    ip->octetsMask[0] = (uint8_t)(mask >> 24);
    ip->octetsMask[1] = (uint8_t)(mask >> 16);
    ip->octetsMask[2] = (uint8_t)(mask >> 8);
    ip->octetsMask[3] =  (uint8_t)mask;
}

void computeNetworkAddress(InfoIP *ip) {
    for (int i = 0; i < 4; i++) {
        ip->octetsNetworkAddress[i] = ip->octetsIP[i] & ip->octetsMask[i];
    }
}

void computeHostCount(InfoIP *ip) {
    if (ip->cidr == 32) {
        ip->nbHosts = 1;
    } else if (ip->cidr == 31) {
        ip->nbHosts = 2;
    } else {
        ip->nbHosts = (1L << ip->hostBits) - 2;
    }
}

uint32_t setHostbits(int bitsHost) {
    if (bitsHost == 0) {
        return 0;
    }
    return 0xFFFFFFFFu >> (32 - bitsHost);
}

void computeBroadcast(InfoIP *ip){
    uint32_t hostBitsSet = setHostbits(ip->hostBits);

    ip->octetsBroadcast[0] = ip->octetsNetworkAddress[0] | (uint8_t)(hostBitsSet >> 24);
    ip->octetsBroadcast[1] = ip->octetsNetworkAddress[1] | (uint8_t)(hostBitsSet >> 16);
    ip->octetsBroadcast[2] = ip->octetsNetworkAddress[2] | (uint8_t)(hostBitsSet >> 8);
    ip->octetsBroadcast[3] = ip->octetsNetworkAddress[3] | (uint8_t)(hostBitsSet);


}


void printIP(const InfoIP *ip) {
    printf("IP              : %" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "/%d\n",ip->octetsIP[0], ip->octetsIP[1], ip->octetsIP[2], ip->octetsIP[3], ip->cidr);
    printf("Decimal Mask    : %" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "\n",ip->octetsMask[0], ip->octetsMask[1], ip->octetsMask[2], ip->octetsMask[3]);

    if (ip->type == PUBLIC)
        printf("Type            : PUBLIC IP\n");
    else
        printf("Type            : PRIVATE IP\n");

    printf("Network Bits    : %d\n", ip->networkBits);
    printf("Host Bits       : %d\n", ip->hostBits);
    printf("Network Address : %" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "/%d\n",ip->octetsNetworkAddress[0], ip->octetsNetworkAddress[1],ip->octetsNetworkAddress[2], ip->octetsNetworkAddress[3], ip->cidr);
    printf("Usable Hosts    : %ld\n", ip->nbHosts);
    printf("Broadcast       : %" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "\n",ip->octetsBroadcast[0], ip->octetsBroadcast[1], ip->octetsBroadcast[2], ip->octetsBroadcast[3]);
}

void printUsage(const char *programName) {
    printf("Incorrect number of arguments.\n");
    printf("Usage example: %s 192.168.5.189/24\n", programName);
}
