/* 
 * File:   tcp-headers.h
 * Author: root
 *
 * Created on August 27, 2012, 1:49 PM
 */

#ifndef UDP_HEADERS_H
#define	UDP_HEADERS_H

#include <pcap.h>
#include <netinet/in.h>

#include <vector>
#include <iostream>
using namespace std;

#include "GenericPacket.h"


/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

// In a computer, it's LSB first, aligned to the byte.
// http://www.erg.abdn.ac.uk/~gorry/eg3561/lan-pages/mac.html

/* Ethernet header */
struct sniff_ethernet {
    u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
    u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
    u_short ether_type;                     /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
    u_char  ip_header  : 4;         /* header */ // These are swapped, LSB first
    u_char  ip_version : 4;         /* version */
    u_char  ip_tos;                 /* type of service */
    u_short ip_len;                 /* total length */
    u_short ip_id;                  /* identification */
    u_short ip_off : 13;            /* fragment offset field */ // LSB first
    u_char  ip_more_fragments : 1;         /* more fragments flag */
    u_char  ip_dont_fragment : 1;         /* dont fragment flag */
    u_char  ip_reserved_bit : 1;     /* reserved fragment flag */
    u_char  ip_ttl;                 /* time to live */
    u_char  ip_p;                   /* protocol */
    u_short ip_sum;                 /* checksum */
    struct  in_addr ip_src, ip_dst;  /* source and dest address */
};

/* UDP headers */
struct sniff_udp {
    u_short ud_sport;
    u_short ud_dport;
    u_short ud_length;
    u_short ud_sum;
};

class UDPPacket : public GenericPacket {
public:
    // Clone constructor
    UDPPacket(){}
    UDPPacket(GenericPacket* packet);
    
    const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
    const struct sniff_ip *ip;              /* The IP header */
    const struct sniff_udp *udp;            /* The UDP header */
    const u_char *payload;                  /* Packet payload */
    
    int size_ip;
    u_short size_udp;
    int size_payload;
    
    // In the modified version of libpcap this is struct timespec
    // struct timespec timestamp;
    struct timeval timestamp;
    
    // string packet_type;-
    bool processPacket(const struct pcap_pkthdr *header, const u_char *packet);
    
    time_t time;
    int ns_timestamp;
};

#endif	/* UDP_HEADERS_H */

