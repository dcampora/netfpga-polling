/* 
 * File:   tcp-headers.h
 * Author: root
 *
 * Created on August 27, 2012, 1:49 PM
 */

#ifndef TCP_HEADERS_H
#define	TCP_HEADERS_H

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

// These functions are precisely ntohs and ntohl
// #define SWAP_16(_v) (_v<<8 & 0xFF00) | (_v>>8 & 0xFF)
// #define SWAP_32(_v) (_v<<24 & 0xFF000000) | _v<<8 & 0xFF0000) | _v>>8 & 0xFF00) | (_v>>24 & 0xFF)

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

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
        u_short th_sport;               /* source port */
        u_short th_dport;               /* destination port */
        tcp_seq th_seq;                 /* sequence number */
        tcp_seq th_ack;                 /* acknowledgement number */
        u_char  th_reserved : 4;        /* reserved */
        u_char  th_offset : 4;          /* data offset */
        u_char  th_f_fin : 1;           /* Flags */ // LSB first
        u_char  th_f_syn : 1;
        u_char  th_f_reset : 1;
        u_char  th_f_push : 1;
        u_char  th_f_ack : 1;
        u_char  th_f_urgent : 1;
        u_char  th_f_ece : 1;
        u_char  th_f_cwr : 1;
        u_short th_win;                 /* window */
        u_short th_sum;                 /* checksum */
        u_short th_urp;                 /* urgent pointer */
};
#define TH_FLAGS(th) (th.th_cwr<<7 | th.th_ece<<6 | th.th_urgent<<5 | th.th_ack<<4 | \
                      th.th_push<<3 | th.th_reset<<2 | th.th_syn << 1 | th.th_fin)



class TCPPacket : public GenericPacket {
public:
    TCPPacket(){}
    TCPPacket(GenericPacket* packet);
    
    const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
    const struct sniff_ip *ip;              /* The IP header */
    const struct sniff_tcp *tcp;            /* The TCP header */
    const u_char *payload;                    /* Packet payload */
    
    int size_ip;
    int size_tcp;
    int size_payload;
    
    // string packet_type;
    bool processPacket(const struct pcap_pkthdr *header, const u_char *packet);
};

#endif	/* TCP_HEADERS_H */

