#ifndef NETCALC_IP_H
#define NETCALC_IP_H
#include <stdint.h>

// Holds all the computed info about an IP/CIDR
typedef struct {
    uint8_t octetsIP[4];
    uint8_t octetsMask[4];
    uint8_t octetsNetworkAddress[4];
    uint8_t octetsBroadcast[4];
    int cidr;
    int networkBits;
    int hostBits;
    long nbHosts;
    int type; // Private or Public
} InfoIP;

// Result of parsing/computing an IP: success or type of error
typedef enum {
    IP_OK = 0,
    IP_ERR_FORMAT = 1,
    IP_ERR_CIDR = 2
} IPStatus;

#define CIDR_MIN 0
#define CIDR_MAX 32
#define PUBLIC 0
#define PRIVATE 1

// Parse a string like "192.168.1.1/24" and fill the struct with the IP and CIDR
IPStatus parseStringIP(InfoIP *ip, const char *string);

// Parse the input and compute the mask, network address, host count and type
IPStatus computeIP(InfoIP *ip, const char *string);

// Build a 32-bit mask with the first "cidr" bits set to 1
uint32_t maskFromCidr(int cidr);

// Convert the 32-bit mask into 4 separate bytes (octets)
void computeMask(InfoIP *ip);

// Compute the network address by applying the mask to the IP
void computeNetworkAddress(InfoIP *ip);

// Compute the number of usable hosts in the network
void computeHostCount(InfoIP *ip);

// Check the IP and set its type to PUBLIC or PRIVATE
void defineType(InfoIP *ip);

// Build a 32-bit mask with all the host bits to 1
uint32_t setHostbits(int bitsHost);

// Compute the broadcast address
void computeBroadcast(InfoIP *ip);

// Print the computed IP info / usage instructions
void printIP(const InfoIP *ip);
void printUsage(const char *programName);

#endif //NETCALC_IP_H
