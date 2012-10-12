/* 
 * File:   mep_headers.h
 * Author: root
 *
 * Created on August 27, 2012, 4:49 PM
 */

#ifndef MEP_HEADERS_H
#define	MEP_HEADERS_H

#include "udp_headers.h"

/*
 * A MEP request is a minimal legal Ethernet packet
 * The length of the IP payload is hence 44 bytes
 */
#define MEP_REQ_LEN 44
struct MEPReq
{
  u_int8_t nmep;   /* number of meps requested */
  u_int32_t runnumber; /* last seen run-number */
  u_int32_t seqno; /* sequence number of this MEP request */
  u_int8_t reserved[MEP_REQ_LEN - sizeof(u_int8_t) - 2 * sizeof(u_int32_t)];
};

class MEPPacket : public GenericPacket {
public:
    // Clone constructor
    MEPPacket(){}
    MEPPacket(GenericPacket* packet);
    
    const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
    const struct sniff_ip *ip;              /* The IP header */
    const struct MEPReq *mep;               /* The MEP header */
    
    int size_ip;
    int size_mep;
    
    // In the modified version of libpcap this is struct timespec
    struct timespec timestamp;
    // struct timeval timestamp;
    
    // string packet_type;
    bool processPacket(const struct pcap_pkthdr *header, const u_char *packet);
    
    time_t time;
};

#endif	/* MEP_HEADERS_H */

